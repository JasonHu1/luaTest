#include "tuya_proto.h"
#include "app_debug_printf.h"

/*
 * Test classes
 */

bool g_success = true;

bool testSucceeded ()
{
  bool b = g_success;
  g_success = false;
  return b;
}

typedef int fn_type;
enum {
  FN_CTOR,
  FN_DTOR,
  FN_STATIC,
  FN_VIRTUAL,
  FN_PROPGET,
  FN_PROPSET,
  FN_STATIC_PROPGET,
  FN_STATIC_PROPSET,
  FN_OPERATOR,
  NUM_FN_TYPES
};

struct fn_called {
  bool called [NUM_FN_TYPES];
  fn_called () { memset(called, 0, NUM_FN_TYPES * sizeof(bool)); }
};

fn_called A_functions, B_functions;

bool testAFnCalled (fn_type f)
{
  bool b = A_functions.called[f];
  A_functions.called [f] = false;
  return b;
}

bool testBFnCalled (fn_type f)
{
  bool b = B_functions.called[f];
  B_functions.called [f] = false;
  return b;
}
#ifdef __cplusplus
extern "C" {
#endif

int sendPtr(TuyaProtoElement*pp){
    vDBG_ERR("111");
    vDBG_INFO("dp.value.m_dpid=%d",pp[0].m_dpid);
    vDBG_INFO("dp.value.m_dpid=%d",pp[1].m_dpid);
    vDBG_INFO("dp.value.m_dpid=%d",pp[2].m_dpid);
}
#ifdef __cplusplus
}
#endif

class A
{
protected:
  string name;
  mutable bool success;
public:
  A (string const& name_) : name (name_), success (false), testProp (47)
  {
    A_functions.called [FN_CTOR] = true;
  }
  virtual ~A ()
  {
    A_functions.called [FN_DTOR] = true;
  }

  virtual void testVirtual ()
  {
    A_functions.called [FN_VIRTUAL] = true;
  }

  const char * getName () const
  {
    return name.c_str();
  }

  void setSuccess () const
  {
    success = true;
  }

  bool testSucceeded () const
  {
    bool b = success;
    success = false;
    return b;
  }

  static void testStatic ()
  {
    A_functions.called [FN_STATIC] = true;
  }

  int testProp;
  int testPropGet () const
  {
    A_functions.called [FN_PROPGET] = true;
    return testProp;
  }
  void testPropSet (int x)
  {
    A_functions.called [FN_PROPSET] = true;
    testProp = x;
  }

  static int testStaticProp;
  static int testStaticPropGet ()
  {
    A_functions.called [FN_STATIC_PROPGET] = true;
    return testStaticProp;
  }
  static void testStaticPropSet (int x)
  {
    A_functions.called [FN_STATIC_PROPSET] = true;
    testStaticProp = x;
  }
  
//  RefCountedPtr <A> operator + (A const& other)
//  {
//    A_functions.called [FN_OPERATOR] = true;
//    return new A (name + " + " + other.name);
//  }
};

int A::testStaticProp = 47;

class B: public A
{
public:
  explicit B (string const& name_) : A (name_)
  {
    B_functions.called [FN_CTOR] = true;
  }

  virtual ~B ()
  {
    B_functions.called [FN_DTOR] = true;
  }

  virtual void testVirtual ()
  {
    B_functions.called [FN_VIRTUAL] = true;
  }

  static void testStatic2 ()
  {
    B_functions.called [FN_STATIC] = true;
  }

};

