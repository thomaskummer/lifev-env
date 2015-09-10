#----------------------------------------------------------------
# Generated CMake target import file for configuration "RELEASE".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "teuchoscore" for configuration "RELEASE"
set_property(TARGET teuchoscore APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(teuchoscore PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libteuchoscore.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS teuchoscore )
list(APPEND _IMPORT_CHECK_FILES_FOR_teuchoscore "${_IMPORT_PREFIX}/lib/libteuchoscore.a" )

# Import target "teuchosparameterlist" for configuration "RELEASE"
set_property(TARGET teuchosparameterlist APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(teuchosparameterlist PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libteuchosparameterlist.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS teuchosparameterlist )
list(APPEND _IMPORT_CHECK_FILES_FOR_teuchosparameterlist "${_IMPORT_PREFIX}/lib/libteuchosparameterlist.a" )

# Import target "teuchoscomm" for configuration "RELEASE"
set_property(TARGET teuchoscomm APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(teuchoscomm PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "teuchosparameterlist;teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libteuchoscomm.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS teuchoscomm )
list(APPEND _IMPORT_CHECK_FILES_FOR_teuchoscomm "${_IMPORT_PREFIX}/lib/libteuchoscomm.a" )

# Import target "teuchosnumerics" for configuration "RELEASE"
set_property(TARGET teuchosnumerics APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(teuchosnumerics PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "teuchoscomm;teuchoscore;/usr/lib64/liblapack.so;/usr/lib64/libblas.so"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libteuchosnumerics.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS teuchosnumerics )
list(APPEND _IMPORT_CHECK_FILES_FOR_teuchosnumerics "${_IMPORT_PREFIX}/lib/libteuchosnumerics.a" )

# Import target "teuchosremainder" for configuration "RELEASE"
set_property(TARGET teuchosremainder APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(teuchosremainder PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libteuchosremainder.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS teuchosremainder )
list(APPEND _IMPORT_CHECK_FILES_FOR_teuchosremainder "${_IMPORT_PREFIX}/lib/libteuchosremainder.a" )

# Import target "tpi" for configuration "RELEASE"
set_property(TARGET tpi APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(tpi PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtpi.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS tpi )
list(APPEND _IMPORT_CHECK_FILES_FOR_tpi "${_IMPORT_PREFIX}/lib/libtpi.a" )

# Import target "sacado" for configuration "RELEASE"
set_property(TARGET sacado APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(sacado PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libsacado.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS sacado )
list(APPEND _IMPORT_CHECK_FILES_FOR_sacado "${_IMPORT_PREFIX}/lib/libsacado.a" )

# Import target "rtop" for configuration "RELEASE"
set_property(TARGET rtop APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(rtop PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/librtop.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS rtop )
list(APPEND _IMPORT_CHECK_FILES_FOR_rtop "${_IMPORT_PREFIX}/lib/librtop.a" )

# Import target "kokkos" for configuration "RELEASE"
set_property(TARGET kokkos APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(kokkos PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "tpi;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libkokkos.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS kokkos )
list(APPEND _IMPORT_CHECK_FILES_FOR_kokkos "${_IMPORT_PREFIX}/lib/libkokkos.a" )

# Import target "kokkosnodeapi" for configuration "RELEASE"
set_property(TARGET kokkosnodeapi APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(kokkosnodeapi PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "kokkos"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libkokkosnodeapi.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS kokkosnodeapi )
list(APPEND _IMPORT_CHECK_FILES_FOR_kokkosnodeapi "${_IMPORT_PREFIX}/lib/libkokkosnodeapi.a" )

# Import target "kokkoslinalg" for configuration "RELEASE"
set_property(TARGET kokkoslinalg APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(kokkoslinalg PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "tpi;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libkokkoslinalg.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS kokkoslinalg )
list(APPEND _IMPORT_CHECK_FILES_FOR_kokkoslinalg "${_IMPORT_PREFIX}/lib/libkokkoslinalg.a" )

# Import target "kokkosnodetsqr" for configuration "RELEASE"
set_property(TARGET kokkosnodetsqr APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(kokkosnodetsqr PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "kokkos"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libkokkosnodetsqr.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS kokkosnodetsqr )
list(APPEND _IMPORT_CHECK_FILES_FOR_kokkosnodetsqr "${_IMPORT_PREFIX}/lib/libkokkosnodetsqr.a" )

# Import target "kokkosdisttsqr" for configuration "RELEASE"
set_property(TARGET kokkosdisttsqr APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(kokkosdisttsqr PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "kokkos;kokkosnodetsqr"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libkokkosdisttsqr.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS kokkosdisttsqr )
list(APPEND _IMPORT_CHECK_FILES_FOR_kokkosdisttsqr "${_IMPORT_PREFIX}/lib/libkokkosdisttsqr.a" )

# Import target "epetra" for configuration "RELEASE"
set_property(TARGET epetra APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(epetra PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX;Fortran"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "/usr/lib64/liblapack.so;/usr/lib64/libblas.so"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libepetra.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS epetra )
list(APPEND _IMPORT_CHECK_FILES_FOR_epetra "${_IMPORT_PREFIX}/lib/libepetra.a" )

# Import target "zoltan" for configuration "RELEASE"
set_property(TARGET zoltan APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(zoltan PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "m;/usr/lib64/libz.so;/local/kummerth/lifev-env/ParMetis-3.2.0/libparmetis.a;/local/kummerth/lifev-env/ParMetis-3.2.0/libmetis.a"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libzoltan.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS zoltan )
list(APPEND _IMPORT_CHECK_FILES_FOR_zoltan "${_IMPORT_PREFIX}/lib/libzoltan.a" )

# Import target "triutils" for configuration "RELEASE"
set_property(TARGET triutils APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(triutils PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "epetra"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtriutils.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS triutils )
list(APPEND _IMPORT_CHECK_FILES_FOR_triutils "${_IMPORT_PREFIX}/lib/libtriutils.a" )

# Import target "tpetra" for configuration "RELEASE"
set_property(TARGET tpetra APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(tpetra PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "epetra;kokkosdisttsqr;kokkosnodetsqr;kokkoslinalg;kokkosnodeapi;kokkos;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtpetra.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS tpetra )
list(APPEND _IMPORT_CHECK_FILES_FOR_tpetra "${_IMPORT_PREFIX}/lib/libtpetra.a" )

# Import target "tpetrainout" for configuration "RELEASE"
set_property(TARGET tpetrainout APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(tpetrainout PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "tpetra"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtpetrainout.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS tpetrainout )
list(APPEND _IMPORT_CHECK_FILES_FOR_tpetrainout "${_IMPORT_PREFIX}/lib/libtpetrainout.a" )

# Import target "tpetraext" for configuration "RELEASE"
set_property(TARGET tpetraext APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(tpetraext PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "tpetra"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libtpetraext.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS tpetraext )
list(APPEND _IMPORT_CHECK_FILES_FOR_tpetraext "${_IMPORT_PREFIX}/lib/libtpetraext.a" )

# Import target "epetraext" for configuration "RELEASE"
set_property(TARGET epetraext APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(epetraext PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "triutils;epetra;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore;/usr/lib64/libhdf5.so;/usr/lib64/libz.so;/usr/lib64/libamd.so;/usr/lib64/libumfpack.so;/usr/lib64/libamd.so"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libepetraext.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS epetraext )
list(APPEND _IMPORT_CHECK_FILES_FOR_epetraext "${_IMPORT_PREFIX}/lib/libepetraext.a" )

# Import target "xpetra" for configuration "RELEASE"
set_property(TARGET xpetra APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(xpetra PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "epetraext;tpetraext;tpetrainout;tpetra;epetra;kokkosdisttsqr;kokkosnodetsqr;kokkoslinalg;kokkosnodeapi;kokkos;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore;/usr/lib64/liblapack.so;/usr/lib64/libblas.so"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libxpetra.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS xpetra )
list(APPEND _IMPORT_CHECK_FILES_FOR_xpetra "${_IMPORT_PREFIX}/lib/libxpetra.a" )

# Import target "xpetra-ext" for configuration "RELEASE"
set_property(TARGET xpetra-ext APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(xpetra-ext PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "xpetra"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libxpetra-ext.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS xpetra-ext )
list(APPEND _IMPORT_CHECK_FILES_FOR_xpetra-ext "${_IMPORT_PREFIX}/lib/libxpetra-ext.a" )

# Import target "xpetra-sup" for configuration "RELEASE"
set_property(TARGET xpetra-sup APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(xpetra-sup PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "xpetra"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libxpetra-sup.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS xpetra-sup )
list(APPEND _IMPORT_CHECK_FILES_FOR_xpetra-sup "${_IMPORT_PREFIX}/lib/libxpetra-sup.a" )

# Import target "thyracore" for configuration "RELEASE"
set_property(TARGET thyracore APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(thyracore PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "rtop;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libthyracore.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS thyracore )
list(APPEND _IMPORT_CHECK_FILES_FOR_thyracore "${_IMPORT_PREFIX}/lib/libthyracore.a" )

# Import target "thyraepetra" for configuration "RELEASE"
set_property(TARGET thyraepetra APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(thyraepetra PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "thyracore;epetra"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libthyraepetra.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS thyraepetra )
list(APPEND _IMPORT_CHECK_FILES_FOR_thyraepetra "${_IMPORT_PREFIX}/lib/libthyraepetra.a" )

# Import target "thyraepetraext" for configuration "RELEASE"
set_property(TARGET thyraepetraext APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(thyraepetraext PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "thyraepetra;thyracore;epetraext;epetra"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libthyraepetraext.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS thyraepetraext )
list(APPEND _IMPORT_CHECK_FILES_FOR_thyraepetraext "${_IMPORT_PREFIX}/lib/libthyraepetraext.a" )

# Import target "thyratpetra" for configuration "RELEASE"
set_property(TARGET thyratpetra APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(thyratpetra PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "thyraepetra;thyracore;tpetraext;tpetrainout;tpetra"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libthyratpetra.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS thyratpetra )
list(APPEND _IMPORT_CHECK_FILES_FOR_thyratpetra "${_IMPORT_PREFIX}/lib/libthyratpetra.a" )

# Import target "isorropia" for configuration "RELEASE"
set_property(TARGET isorropia APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(isorropia PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "epetraext;tpetraext;tpetrainout;tpetra;zoltan;epetra;kokkosdisttsqr;kokkosnodetsqr;kokkoslinalg;kokkosnodeapi;kokkos;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libisorropia.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS isorropia )
list(APPEND _IMPORT_CHECK_FILES_FOR_isorropia "${_IMPORT_PREFIX}/lib/libisorropia.a" )

# Import target "aztecoo" for configuration "RELEASE"
set_property(TARGET aztecoo APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(aztecoo PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX;Fortran"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "triutils;epetra;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libaztecoo.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS aztecoo )
list(APPEND _IMPORT_CHECK_FILES_FOR_aztecoo "${_IMPORT_PREFIX}/lib/libaztecoo.a" )

# Import target "galeri" for configuration "RELEASE"
set_property(TARGET galeri APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(galeri PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "xpetra-sup;xpetra-ext;xpetra;epetraext;tpetraext;tpetrainout;tpetra;epetra;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libgaleri.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS galeri )
list(APPEND _IMPORT_CHECK_FILES_FOR_galeri "${_IMPORT_PREFIX}/lib/libgaleri.a" )

# Import target "galeri-xpetra" for configuration "RELEASE"
set_property(TARGET galeri-xpetra APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(galeri-xpetra PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "xpetra-sup;xpetra-ext;xpetra;epetraext;tpetraext;tpetrainout;tpetra;epetra;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libgaleri-xpetra.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS galeri-xpetra )
list(APPEND _IMPORT_CHECK_FILES_FOR_galeri-xpetra "${_IMPORT_PREFIX}/lib/libgaleri-xpetra.a" )

# Import target "amesos" for configuration "RELEASE"
set_property(TARGET amesos APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(amesos PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "epetraext;epetra;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore;/usr/lib64/libumfpack.so;/usr/lib64/libamd.so;/local/kummerth/lifev-env/ParMetis-3.2.0/libparmetis.a;/local/kummerth/lifev-env/ParMetis-3.2.0/libmetis.a"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libamesos.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS amesos )
list(APPEND _IMPORT_CHECK_FILES_FOR_amesos "${_IMPORT_PREFIX}/lib/libamesos.a" )

# Import target "ifpack" for configuration "RELEASE"
set_property(TARGET ifpack APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(ifpack PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "amesos;aztecoo;epetraext;epetra;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libifpack.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS ifpack )
list(APPEND _IMPORT_CHECK_FILES_FOR_ifpack "${_IMPORT_PREFIX}/lib/libifpack.a" )

# Import target "ml" for configuration "RELEASE"
set_property(TARGET ml APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(ml PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "ifpack;amesos;galeri-xpetra;galeri;aztecoo;isorropia;epetraext;zoltan;epetra;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore;/local/kummerth/lifev-env/ParMetis-3.2.0/libparmetis.a;/local/kummerth/lifev-env/ParMetis-3.2.0/libmetis.a;/usr/lib64/liblapack.so;/usr/lib64/libblas.so"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libml.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS ml )
list(APPEND _IMPORT_CHECK_FILES_FOR_ml "${_IMPORT_PREFIX}/lib/libml.a" )

# Import target "belos" for configuration "RELEASE"
set_property(TARGET belos APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(belos PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "kokkos;kokkosnodetsqr;kokkosdisttsqr;tpetraext;tpetrainout;tpetra;epetra;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libbelos.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS belos )
list(APPEND _IMPORT_CHECK_FILES_FOR_belos "${_IMPORT_PREFIX}/lib/libbelos.a" )

# Import target "belosepetra" for configuration "RELEASE"
set_property(TARGET belosepetra APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(belosepetra PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "belos;epetra;tpetra;kokkos;kokkosnodetsqr;kokkosdisttsqr"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libbelosepetra.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS belosepetra )
list(APPEND _IMPORT_CHECK_FILES_FOR_belosepetra "${_IMPORT_PREFIX}/lib/libbelosepetra.a" )

# Import target "belostpetra" for configuration "RELEASE"
set_property(TARGET belostpetra APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(belostpetra PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "belos;tpetra;kokkos;kokkosnodetsqr;kokkosdisttsqr"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libbelostpetra.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS belostpetra )
list(APPEND _IMPORT_CHECK_FILES_FOR_belostpetra "${_IMPORT_PREFIX}/lib/libbelostpetra.a" )

# Import target "anasazi" for configuration "RELEASE"
set_property(TARGET anasazi APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(anasazi PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "thyraepetra;thyracore;tpetraext;tpetrainout;tpetra;epetra;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libanasazi.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS anasazi )
list(APPEND _IMPORT_CHECK_FILES_FOR_anasazi "${_IMPORT_PREFIX}/lib/libanasazi.a" )

# Import target "anasaziepetra" for configuration "RELEASE"
set_property(TARGET anasaziepetra APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(anasaziepetra PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "anasazi"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libanasaziepetra.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS anasaziepetra )
list(APPEND _IMPORT_CHECK_FILES_FOR_anasaziepetra "${_IMPORT_PREFIX}/lib/libanasaziepetra.a" )

# Import target "ModeLaplace" for configuration "RELEASE"
set_property(TARGET ModeLaplace APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(ModeLaplace PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "anasazi"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libModeLaplace.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS ModeLaplace )
list(APPEND _IMPORT_CHECK_FILES_FOR_ModeLaplace "${_IMPORT_PREFIX}/lib/libModeLaplace.a" )

# Import target "anasazitpetra" for configuration "RELEASE"
set_property(TARGET anasazitpetra APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(anasazitpetra PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "anasazi;tpetra;kokkos;kokkosnodetsqr;kokkosdisttsqr"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libanasazitpetra.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS anasazitpetra )
list(APPEND _IMPORT_CHECK_FILES_FOR_anasazitpetra "${_IMPORT_PREFIX}/lib/libanasazitpetra.a" )

# Import target "shylu" for configuration "RELEASE"
set_property(TARGET shylu APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(shylu PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "belostpetra;belosepetra;belos;ifpack;amesos;aztecoo;isorropia;epetra;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libshylu.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS shylu )
list(APPEND _IMPORT_CHECK_FILES_FOR_shylu "${_IMPORT_PREFIX}/lib/libshylu.a" )

# Import target "stratimikosifpack" for configuration "RELEASE"
set_property(TARGET stratimikosifpack APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(stratimikosifpack PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "belostpetra;belosepetra;belos;ml;ifpack;amesos;aztecoo;thyraepetra;epetraext"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libstratimikosifpack.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS stratimikosifpack )
list(APPEND _IMPORT_CHECK_FILES_FOR_stratimikosifpack "${_IMPORT_PREFIX}/lib/libstratimikosifpack.a" )

# Import target "stratimikosml" for configuration "RELEASE"
set_property(TARGET stratimikosml APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(stratimikosml PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "belostpetra;belosepetra;belos;ml;ifpack;amesos;aztecoo;thyraepetra;epetraext"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libstratimikosml.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS stratimikosml )
list(APPEND _IMPORT_CHECK_FILES_FOR_stratimikosml "${_IMPORT_PREFIX}/lib/libstratimikosml.a" )

# Import target "stratimikosamesos" for configuration "RELEASE"
set_property(TARGET stratimikosamesos APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(stratimikosamesos PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "belostpetra;belosepetra;belos;ml;ifpack;amesos;aztecoo;thyraepetra;epetraext"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libstratimikosamesos.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS stratimikosamesos )
list(APPEND _IMPORT_CHECK_FILES_FOR_stratimikosamesos "${_IMPORT_PREFIX}/lib/libstratimikosamesos.a" )

# Import target "stratimikosaztecoo" for configuration "RELEASE"
set_property(TARGET stratimikosaztecoo APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(stratimikosaztecoo PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "belostpetra;belosepetra;belos;ml;ifpack;amesos;aztecoo;thyraepetra;epetraext"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libstratimikosaztecoo.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS stratimikosaztecoo )
list(APPEND _IMPORT_CHECK_FILES_FOR_stratimikosaztecoo "${_IMPORT_PREFIX}/lib/libstratimikosaztecoo.a" )

# Import target "stratimikosbelos" for configuration "RELEASE"
set_property(TARGET stratimikosbelos APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(stratimikosbelos PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "belostpetra;belosepetra;belos;ml;ifpack;amesos;aztecoo;thyraepetra;epetraext"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libstratimikosbelos.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS stratimikosbelos )
list(APPEND _IMPORT_CHECK_FILES_FOR_stratimikosbelos "${_IMPORT_PREFIX}/lib/libstratimikosbelos.a" )

# Import target "stratimikos" for configuration "RELEASE"
set_property(TARGET stratimikos APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(stratimikos PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "stratimikosamesos;stratimikosaztecoo;stratimikosbelos;stratimikosifpack;stratimikosml"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libstratimikos.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS stratimikos )
list(APPEND _IMPORT_CHECK_FILES_FOR_stratimikos "${_IMPORT_PREFIX}/lib/libstratimikos.a" )

# Import target "teko" for configuration "RELEASE"
set_property(TARGET teko APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(teko PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "stratimikos;stratimikosbelos;stratimikosaztecoo;stratimikosamesos;stratimikosml;stratimikosifpack;anasazitpetra;ModeLaplace;anasaziepetra;anasazi;ml;ifpack;amesos;aztecoo;isorropia;thyratpetra;thyraepetraext;thyraepetra;thyracore;thyraepetraext;thyraepetra;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libteko.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS teko )
list(APPEND _IMPORT_CHECK_FILES_FOR_teko "${_IMPORT_PREFIX}/lib/libteko.a" )

# Import target "nox" for configuration "RELEASE"
set_property(TARGET nox APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(nox PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "teko;stratimikos;stratimikosbelos;stratimikosaztecoo;stratimikosamesos;stratimikosml;stratimikosifpack;anasazitpetra;ModeLaplace;anasaziepetra;anasazi;belostpetra;belosepetra;belos;ml;ifpack;amesos;aztecoo;thyraepetraext;thyraepetra;thyracore;epetraext;epetra;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore;/usr/lib64/liblapack.so;/usr/lib64/libblas.so"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libnox.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS nox )
list(APPEND _IMPORT_CHECK_FILES_FOR_nox "${_IMPORT_PREFIX}/lib/libnox.a" )

# Import target "noxlapack" for configuration "RELEASE"
set_property(TARGET noxlapack APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(noxlapack PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "nox"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libnoxlapack.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS noxlapack )
list(APPEND _IMPORT_CHECK_FILES_FOR_noxlapack "${_IMPORT_PREFIX}/lib/libnoxlapack.a" )

# Import target "noxepetra" for configuration "RELEASE"
set_property(TARGET noxepetra APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(noxepetra PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "nox"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libnoxepetra.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS noxepetra )
list(APPEND _IMPORT_CHECK_FILES_FOR_noxepetra "${_IMPORT_PREFIX}/lib/libnoxepetra.a" )

# Import target "loca" for configuration "RELEASE"
set_property(TARGET loca APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(loca PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "nox"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libloca.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS loca )
list(APPEND _IMPORT_CHECK_FILES_FOR_loca "${_IMPORT_PREFIX}/lib/libloca.a" )

# Import target "localapack" for configuration "RELEASE"
set_property(TARGET localapack APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(localapack PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "loca;noxlapack;nox"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/liblocalapack.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS localapack )
list(APPEND _IMPORT_CHECK_FILES_FOR_localapack "${_IMPORT_PREFIX}/lib/liblocalapack.a" )

# Import target "locaepetra" for configuration "RELEASE"
set_property(TARGET locaepetra APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(locaepetra PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "loca;noxepetra;nox"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/liblocaepetra.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS locaepetra )
list(APPEND _IMPORT_CHECK_FILES_FOR_locaepetra "${_IMPORT_PREFIX}/lib/liblocaepetra.a" )

# Import target "locathyra" for configuration "RELEASE"
set_property(TARGET locathyra APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(locathyra PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "loca;nox"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/liblocathyra.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS locathyra )
list(APPEND _IMPORT_CHECK_FILES_FOR_locathyra "${_IMPORT_PREFIX}/lib/liblocathyra.a" )

# Import target "rythmos" for configuration "RELEASE"
set_property(TARGET rythmos APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(rythmos PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "thyracore;teuchosremainder;teuchosnumerics;teuchoscomm;teuchosparameterlist;teuchoscore"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/librythmos.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS rythmos )
list(APPEND _IMPORT_CHECK_FILES_FOR_rythmos "${_IMPORT_PREFIX}/lib/librythmos.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
