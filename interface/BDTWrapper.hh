#ifndef __CLFV_BDTWRAPPER__
#define __CLFV_BDTWRAPPER__

#define PY_SSIZE_T_CLEAN
#include <Python.h>

//c++ includes
#include <iostream>

//local includes
#include "interface/Tree_t.hh"

namespace CLFV {

  class BDTWrapper {
  public:
    BDTWrapper(std::string script = "BDTEval") {
      Py_Initialize();
      PyObject* pModule = PyImport_ImportModule(script.c_str());
      // PyObject* pName = PyString_FromString(script.c_str());
      // PyObject* pModule = PyImport_Import(pName);
      PyObject* pDict = PyModule_GetDict(pModule);
      PyObject* pClass = PyDict_GetItemString(pDict, script.c_str());
      scriptHandle_ = PyObject_CallObject(pClass, nullptr);
    }

    ~BDTWrapper() {
    }

    float EvaluateScore(int index, Tree_t& vars) {
      if(index >= 0 && vars.leponept > 0.f) {
        return 0.f;
      }
      return -2.f;
    }

  private:
    PyObject* pModule_;
    PyObject* scriptHandle_;
  };
}




#endif
