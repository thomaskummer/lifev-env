// @HEADER
//
// ***********************************************************************
//
//   Zoltan2: A package of combinatorial algorithms for scientific computing
//                  Copyright 2012 Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Karen Devine      (kddevin@sandia.gov)
//                    Erik Boman        (egboman@sandia.gov)
//                    Siva Rajamanickam (srajama@sandia.gov)
//
// ***********************************************************************
//
// @HEADER

/*! \file PQJagged.cpp
    \brief An example of partitioning coordinates with PQJagged.
    \todo add more cases to this test.
 */

#include <Zoltan2_TestHelpers.hpp>
#include <Zoltan2_BasicCoordinateInput.hpp>
#include <Zoltan2_XpetraMultiVectorInput.hpp>
#include <Zoltan2_PartitioningSolution.hpp>
#include <Zoltan2_PartitioningProblem.hpp>
#include <GeometricGenerator.hpp>
#include <vector>

#include <Zoltan2_PartitioningSolutionQuality.hpp>

#include "Teuchos_XMLParameterListHelpers.hpp"

#include <Teuchos_LAPACK.hpp>
#include <fstream>
#include <string>
using namespace std;
using Teuchos::RCP;
using Teuchos::rcp;

#define CATCH_EXCEPTIONS(pp) \
        catch (std::runtime_error &e) { \
            cout << "Runtime exception returned from " << pp << ": " \
            << e.what() << " FAIL" << endl; \
            return -1; \
        } \
        catch (std::logic_error &e) { \
            cout << "Logic exception returned from " << pp << ": " \
            << e.what() << " FAIL" << endl; \
            return -1; \
        } \
        catch (std::bad_alloc &e) { \
            cout << "Bad_alloc exception returned from " << pp << ": " \
            << e.what() << " FAIL" << endl; \
            return -1; \
        } \
        catch (std::exception &e) { \
            cout << "Unknown exception returned from " << pp << ": " \
            << e.what() << " FAIL" << endl; \
            return -1; \
        }


typedef Tpetra::MultiVector<scalar_t, lno_t, gno_t, node_t> tMVector_t;
typedef Zoltan2::BasicUserTypes<scalar_t, gno_t, lno_t, gno_t> myTypes_t;


/*! \test PQJaggedTest.cpp
    An example of the use of the PQJagged algorithm to partition coordinate data.
 */


const char param_comment = '#';

string trim_right_copy(
        const string& s,
        const string& delimiters = " \f\n\r\t\v" )
{
    return s.substr( 0, s.find_last_not_of( delimiters ) + 1 );
}

string trim_left_copy(
        const string& s,
        const string& delimiters = " \f\n\r\t\v" )
{
    return s.substr( s.find_first_not_of( delimiters ) );
}

string trim_copy(
        const string& s,
        const string& delimiters = " \f\n\r\t\v" )
{
    return trim_left_copy( trim_right_copy( s, delimiters ), delimiters );
}

void readGeoGenParams(string paramFileName, Teuchos::ParameterList &geoparams, const RCP<const Teuchos::Comm<int> > & comm){
    std::string input = "";
    char inp[25000];
    for(int i = 0; i < 25000; ++i){
        inp[i] = 0;
    }

    bool fail = false;
    if(comm->getRank() == 0){

        fstream inParam(paramFileName.c_str());
        if (inParam.fail())
        {
            fail = true;
        }
        if(!fail)
        {
            std::string tmp = "";
            getline (inParam,tmp);
            while (!inParam.eof()){
                if(tmp != ""){
                    tmp = trim_copy(tmp);
                    if(tmp != ""){
                        input += tmp + "\n";
                    }
                }
                getline (inParam,tmp);
            }
            inParam.close();
            for (size_t i = 0; i < input.size(); ++i){
                inp[i] = input[i];
            }
        }
    }



    int size = input.size();
    if(fail){
        size = -1;
    }
    comm->broadcast(0, sizeof(int), (char*) &size);
    if(size == -1){
        throw "File " + paramFileName + " cannot be opened.";
    }
    comm->broadcast(0, size, inp);
    istringstream inParam(inp);
    string str;
    getline (inParam,str);
    while (!inParam.eof()){
        if(str[0] != param_comment){
            size_t pos = str.find('=');
            if(pos == string::npos){
                throw  "Invalid Line:" + str  + " in parameter file";
            }
            string paramname = trim_copy(str.substr(0,pos));
            string paramvalue = trim_copy(str.substr(pos + 1));
            geoparams.set(paramname, paramvalue);
        }
        getline (inParam,str);
    }
}

int GeometricGen(const RCP<const Teuchos::Comm<int> > & comm,
        partId_t numParts, float imbalance,
        std::string paramFile, std::string pqParts,
        std::string pfname,
        partId_t k,


        int migration_check_option,
        int migration_all_to_all_type,
        scalar_t migration_imbalance_cut_off,
        int migration_processor_assignment_type,
        int migration_doMigration_type

)
{

    Teuchos::ParameterList geoparams("geo params");
    readGeoGenParams(paramFile, geoparams, comm);
    GeometricGenerator<scalar_t, lno_t, gno_t, node_t> *gg = new GeometricGenerator<scalar_t, lno_t, gno_t, node_t>(geoparams,comm);

    int coord_dim = gg->getCoordinateDimension();
    int weight_dim = gg->getWeightDimension();
    lno_t numLocalPoints = gg->getNumLocalCoords(); gno_t numGlobalPoints = gg->getNumGlobalCoords();
    scalar_t **coords = new scalar_t * [coord_dim];
    for(int i = 0; i < coord_dim; ++i){
        coords[i] = new scalar_t[numLocalPoints];
    }
    gg->getLocalCoordinatesCopy(coords);
    scalar_t **weight = NULL;
    if(weight_dim){
        weight= new scalar_t * [weight_dim];
        for(int i = 0; i < weight_dim; ++i){
            weight[i] = new scalar_t[numLocalPoints];
        }
        gg->getLocalWeightsCopy(weight);
    }

    delete gg;

    RCP<Tpetra::Map<lno_t, gno_t, node_t> > mp = rcp(
            new Tpetra::Map<lno_t, gno_t, node_t> (numGlobalPoints, numLocalPoints, 0, comm));

    Teuchos::Array<Teuchos::ArrayView<const scalar_t> > coordView(coord_dim);
    for (int i=0; i < coord_dim; i++){
        if(numLocalPoints > 0){
            Teuchos::ArrayView<const scalar_t> a(coords[i], numLocalPoints);
            coordView[i] = a;
        } else{
            Teuchos::ArrayView<const scalar_t> a;
            coordView[i] = a;
        }
    }

    RCP< Tpetra::MultiVector<scalar_t, lno_t, gno_t, node_t> >tmVector = RCP< Tpetra::MultiVector<scalar_t, lno_t, gno_t, node_t> >(
            new Tpetra::MultiVector<scalar_t, lno_t, gno_t, node_t>( mp, coordView.view(0, coord_dim), coord_dim));


    RCP<const tMVector_t> coordsConst = Teuchos::rcp_const_cast<const tMVector_t>(tmVector);
    vector<const scalar_t *> weights;
    if(weight_dim){
        for (int i = 0; i < weight_dim;++i){
            weights.push_back(weight[i]);
        }
    }
    vector <int> stride;

#if 0
    typedef Zoltan2::BasicCoordinateInput<tMVector_t> inputAdapter_t;
    inputAdapter_t ia(localCount, globalIds, x, y, z, 1, 1, 1);
#else
    typedef Zoltan2::XpetraMultiVectorInput<tMVector_t> inputAdapter_t;
    //inputAdapter_t ia(coordsConst);
    inputAdapter_t ia(coordsConst,weights, stride);
#endif

    Teuchos::RCP <Teuchos::ParameterList> params ;

    //Teuchos::ParameterList params("test params");
    if(pfname != ""){
        params = Teuchos::getParametersFromXmlFile(pfname);
    }
    else {
        params =RCP <Teuchos::ParameterList> (new Teuchos::ParameterList, true);
    }
/*
    params->set("memory_output_stream" , "std::cout");
    params->set("memory_procs" , 0);
    */
    params->set("timer_output_stream" , "std::cout");

    params->set("algorithm", "multijagged");
    params->set("compute_metrics", "true");

    if(imbalance > 1){
        params->set("imbalance_tolerance", double(imbalance));
    }

    if(pqParts != ""){
        params->set("pqParts", pqParts);
    }
    if(numParts > 0){
        params->set("num_global_parts", numParts);
    }
    if (k > 0){
        params->set("parallel_part_calculation_count", k);
    }
    if(migration_processor_assignment_type >= 0){
        params->set("migration_processor_assignment_type", migration_processor_assignment_type);
    }
    if(migration_check_option >= 0){
        params->set("migration_check_option", migration_check_option);
    }
    if(migration_all_to_all_type >= 0){
        params->set("migration_all_to_all_type", migration_all_to_all_type);
    }
    if(migration_imbalance_cut_off >= 0){
        params->set("migration_imbalance_cut_off", double (migration_imbalance_cut_off));
    }
    if (migration_doMigration_type >= 0){
        params->set("migration_doMigration_type", int (migration_doMigration_type));
    }

    Zoltan2::PartitioningProblem<inputAdapter_t> *problem;
    try {
#ifdef HAVE_ZOLTAN2_MPI
        problem = new Zoltan2::PartitioningProblem<inputAdapter_t>(&ia, params.getRawPtr(),
                MPI_COMM_WORLD);
#else
        problem = new Zoltan2::PartitioningProblem<inputAdapter_t>(&ia, params.getRawPtr());
#endif
    }
    CATCH_EXCEPTIONS("PartitioningProblem()")

    try {
        problem->solve();
    }
    CATCH_EXCEPTIONS("solve()")
    if (comm->getRank() == 0){
        problem->printMetrics(cout);
    }
    problem->printTimers();
    if(weight_dim){
        for(int i = 0; i < weight_dim; ++i)
            delete [] weight[i];
        delete [] weight;
    }
    if(coord_dim){
        for(int i = 0; i < coord_dim; ++i)
            delete [] coords[i];
        delete [] coords;
    }
    delete problem;
    return 0;
}

int testFromDataFile(
        const RCP<const Teuchos::Comm<int> > & comm,
        partId_t numParts,
        float imbalance,
        std::string fname,
        std::string pqParts,
        std::string pfname,
        partId_t k,
        int migration_check_option,
        int migration_all_to_all_type,
        scalar_t migration_imbalance_cut_off,
        int migration_processor_assignment_type,
        int migration_doMigration_type
)
{
    //std::string fname("simple");
    //cout << "running " << fname << endl;

    UserInputForTests uinput(testDataFilePath, fname, comm, true);

    RCP<tMVector_t> coords = uinput.getCoordinates();

#if 0
    size_t localCount = coords->getLocalLength();
    int dim = coords->getNumVectors();

    scalar_t *x=NULL, *y=NULL, *z=NULL;
    x = coords->getDataNonConst(0).getRawPtr();

    if (dim > 1){
        y = coords->getDataNonConst(1).getRawPtr();
        if (dim > 2)
            z = coords->getDataNonConst(2).getRawPtr();
    }

    const gno_t *globalIds = coords->getMap()->getNodeElementList().getRawPtr();

    typedef Zoltan2::BasicCoordinateInput<tMVector_t> inputAdapter_t;
    inputAdapter_t ia(localCount, globalIds, x, y, z, 1, 1, 1);
#else
    RCP<const tMVector_t> coordsConst = rcp_const_cast<const tMVector_t>(coords);

    typedef Zoltan2::XpetraMultiVectorInput<tMVector_t> inputAdapter_t;
    inputAdapter_t ia(coordsConst);
#endif

    Teuchos::RCP <Teuchos::ParameterList> params ;

    //Teuchos::ParameterList params("test params");
    if(pfname != ""){
        params = Teuchos::getParametersFromXmlFile(pfname);
    }
    else {
        params =RCP <Teuchos::ParameterList> (new Teuchos::ParameterList, true);
    }

    //params->set("timer_output_stream" , "std::cout");
    params->set("compute_metrics", "true");
    params->set("algorithm", "multijagged");
    if(imbalance > 1){
        params->set("imbalance_tolerance", double(imbalance));
    }

    if(pqParts != ""){
        params->set("pqParts", pqParts);
    }
    if(numParts > 0){
        params->set("num_global_parts", numParts);
    }
    if (k > 0){
        params->set("parallel_part_calculation_count", k);
    }
    if(migration_processor_assignment_type >= 0){
        params->set("migration_processor_assignment_type", migration_processor_assignment_type);
    }
    if(migration_check_option >= 0){
        params->set("migration_check_option", migration_check_option);
    }
    if(migration_all_to_all_type >= 0){
        params->set("migration_all_to_all_type", migration_all_to_all_type);
    }
    if(migration_imbalance_cut_off >= 0){
        params->set("migration_imbalance_cut_off", double (migration_imbalance_cut_off));
    }
    if (migration_doMigration_type >= 0){
        params->set("migration_doMigration_type", int (migration_doMigration_type));
    }

    Zoltan2::PartitioningProblem<inputAdapter_t> *problem;
    try {
#ifdef HAVE_ZOLTAN2_MPI
        problem = new Zoltan2::PartitioningProblem<inputAdapter_t>(&ia, params.getRawPtr(),
                MPI_COMM_WORLD);
#else
        problem = new Zoltan2::PartitioningProblem<inputAdapter_t>(&ia, params.getRawPtr());
#endif
    }
    CATCH_EXCEPTIONS("PartitioningProblem()")

    try {
        problem->solve();
    }
    CATCH_EXCEPTIONS("solve()")

    if (coordsConst->getGlobalLength() < 40) {
        int len = coordsConst->getLocalLength();
        const zoltan2_partId_t *zparts = problem->getSolution().getPartList();
        const gno_t *zgids = problem->getSolution().getIdList();
        for (int i = 0; i < len; i++)
            cout << comm->getRank()
            << " gid " << zgids[i] << " part " << zparts[i] << endl;
    }

    if (comm->getRank() == 0){
        problem->printMetrics(cout);
        cout << "testFromDataFile is done " << endl;
    }

    problem->printTimers();
    delete problem;
    return 0;
}






string convert_to_string(char *args){
    string tmp = "";
    for(int i = 0; args[i] != 0; i++)
        tmp += args[i];
    return tmp;
}
bool getArgumentValue(string &argumentid, double &argumentValue, string argumentline){
    stringstream stream(stringstream::in | stringstream::out);
    stream << argumentline;
    getline(stream, argumentid, '=');
    if (stream.eof()){
        return false;
    }
    stream >> argumentValue;
    return true;
}

void getArgVals(
        int argc,
        char **argv,
        partId_t &numParts,
        float &imbalance ,
        string &pqParts,
        int &opt,
        std::string &fname,
        std::string &pfname,
        partId_t &k,
        int &migration_check_option,
        int &migration_all_to_all_type,
        scalar_t &migration_imbalance_cut_off,
        int &migration_processor_assignment_type,
        int &migration_doMigration_type){

    bool isCset = false;
    bool isPset = false;
    bool isFset = false;
    bool isPFset = false;

    for(int i = 0; i < argc; ++i){
        string tmp = convert_to_string(argv[i]);
        string identifier = "";
        long long int value = -1; double fval = -1;
        if(!getArgumentValue(identifier, fval, tmp)) continue;
        value = (long long int) (fval);

        if(identifier == "C"){
            if(value > 0){
                numParts=value;
                isCset = true;
            } else {
                throw  "Invalid argument at " + tmp;
            }
        } else if(identifier == "P"){
            stringstream stream(stringstream::in | stringstream::out);
            stream << tmp;
            string ttmp;
            getline(stream, ttmp, '=');
            stream >> pqParts;
            isPset = true;
        }else if(identifier == "I"){
            if(fval > 0){
                imbalance=fval;
            } else {
                throw "Invalid argument at " + tmp;
            }
        } else if(identifier == "MI"){
            if(fval > 0){
                migration_imbalance_cut_off=fval;
            } else {
                throw "Invalid argument at " + tmp;
            }
        } else if(identifier == "MO"){
            if(value >=0 ){
                migration_check_option = value;
            } else {
                throw "Invalid argument at " + tmp;
            }
        } else if(identifier == "AT"){
            if(value >=0 ){
                migration_processor_assignment_type = value;
            } else {
                throw "Invalid argument at " + tmp;
            }
        }

        else if(identifier == "MT"){
            if(value >=0 ){
                migration_all_to_all_type = value;
            } else {
                throw "Invalid argument at " + tmp;
            }
        }
        else if(identifier == "DM"){
            if(value >=0 ){
                migration_doMigration_type = value;
            } else {
                throw "Invalid argument at " + tmp;
            }
        }
        else if(identifier == "F"){
            stringstream stream(stringstream::in | stringstream::out);
            stream << tmp;
            getline(stream, fname, '=');

            stream >> fname;
            isFset = true;
        }
        else if(identifier == "PF"){
            stringstream stream(stringstream::in | stringstream::out);
            stream << tmp;
            getline(stream, pfname, '=');

            stream >> pfname;
            isPFset = true;
        }

        else if(identifier == "O"){
            if(value >= 0 && value <= 3){
                opt = value;
            } else {
                throw "Invalid argument at " + tmp;
            }
        }
        else if(identifier == "K"){
            if(value >=0 ){
                k = value;
            } else {
                throw "Invalid argument at " + tmp;
            }
        }
        else {
            throw "Invalid argument at " + tmp;
        }

    }
    if(!( ((isCset && isPset) || isPFset) && isFset)){
        throw "((P && C) || PF) && F are mandatory arguments.";
    }

}

void print_usage(char *executable){
    cout << "\nUsage:" << endl;
    cout << executable << " arglist" << endl;
    cout << "arglist:" << endl;
    cout << "\tC=numParts: numParts > 0" << endl;
    cout << "\tP=pqJaggedPart: Example: P=512,512" << endl;
    cout << "\tI=imbalance: Example I=1.03 (ignored for now.)" << endl;
    cout << "\tF=filePath: When O=0 the path of the coordinate input file, for O>1 the path to the geometric generator parameter file." << endl;
    cout << "\tO=input option: O=0 for reading coordinate from file, O>0 for generating coordinate from coordinate generator file. Default will run geometric generator." << endl;
    cout << "\tK=concurrent part calculation input: K>0." << endl;
    cout << "\tMI=migration_imbalance_cut_off: MI=1.15. " << endl;
    cout << "\tMT=migration_all_to_all_type: 0 for alltoallv, 1 for Zoltan_Comm, 2 for Zoltan2 Distributor object(Default 1)." << endl;
    cout << "\tMO=migration_check_option: 0 for decision on imbalance, 1 for forcing migration, >1 for avoiding migration. (Default-2)" << endl;
    cout << "\tAT=migration_processor_assignment_type. 0-for assigning procs with respect to proc ownment, otherwise, assignment with respect to proc closeness." << endl;
    cout << "Example:\n" << executable << " P=2,2,2 C=8 F=simple O=0" << endl;
}

int main(int argc, char *argv[])
{
    Teuchos::GlobalMPISession session(&argc, &argv);
    //cout << argv << endl;

    RCP<const Teuchos::Comm<int> > tcomm = Teuchos::DefaultComm<int>::getComm();
    int rank = tcomm->getRank();


    partId_t numParts = -10; float imbalance = -1.03;
    partId_t k = -1;

    string pqParts = "";
    int opt = 1;
    std::string fname = "";
    std::string paramFile = "";


    int migration_check_option = -2;
    int migration_all_to_all_type = -1;
    scalar_t migration_imbalance_cut_off = -1.15;
    int migration_processor_assignment_type = -1;
    int migration_doMigration_type = -1;

    try{
        try {
            getArgVals(
                    argc,
                    argv,
                    numParts,
                    imbalance ,
                    pqParts,
                    opt,
                    fname,
                    paramFile,
                    k,
                    migration_check_option,
                    migration_all_to_all_type,
                    migration_imbalance_cut_off,
                    migration_processor_assignment_type,
                    migration_doMigration_type);
        }
        catch(std::string s){
            if(tcomm->getRank() == 0){
                print_usage(argv[0]);
            }
            throw s;
        }

        catch(char * s){
            if(tcomm->getRank() == 0){
                print_usage(argv[0]);
            }
            throw s;
        }
        catch(char const * s){
            if(tcomm->getRank() == 0){
                print_usage(argv[0]);
            }
            throw s;
        }

        int ierr = 0;

        switch (opt){

        case 0:
            ierr = testFromDataFile(tcomm,numParts, imbalance,fname,pqParts, paramFile, k,
                    migration_check_option,
                    migration_all_to_all_type,
                    migration_imbalance_cut_off,
                    migration_processor_assignment_type,
                    migration_doMigration_type);
            break;
        default:
            GeometricGen(tcomm, numParts, imbalance, fname, pqParts, paramFile, k,
                    migration_check_option,
                    migration_all_to_all_type,
                    migration_imbalance_cut_off,
                    migration_processor_assignment_type,
                    migration_doMigration_type);
            break;
        }

        if (rank == 0) {
            if (ierr == 0) std::cout << "PASS" << std::endl;
            else std::cout << "FAIL" << std::endl;
        }
    }


    catch(std::string &s){
        if (rank == 0)
            cerr << s << endl;
    }

    catch(char * s){
        if (rank == 0)
            cerr << s << endl;
    }

    catch(char const* s){
        if (rank == 0)
            cerr << s << endl;
    }

}
