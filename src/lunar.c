

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "lualib.h"
#include "lauxlib.h"
#include "lunar.h"



#define LUA_NEW_METAMETHOD(luastate, obj, funcname) {           \
    lua_pushfstring((luastate), "__%s", (#funcname));           \
    lua_pushcfunction((luastate), (luaC_##obj##__##funcname));  \
    lua_settable((luastate), -3);                               \
  }

#define LUA_NEW_MODULEMETHOD(luastate, obj, funcname) {         \
    lua_pushfstring((luastate), "%s", (#funcname));             \
    lua_pushcfunction((luastate), (luaC_##obj##_##funcname));   \
    lua_settable((luastate), -3);                               \
  }

#define LUA_NEW_MODULEDATA(luastate, obj, dataname) {		\
    lua_pushstring((luastate), (#dataname));			\
    lua_pushnumber((luastate), (obj));				\
    lua_settable((luastate), -3);                               \
  }


static int luaC_lunar_table(lua_State *L);
static int luaC_lunar_array(lua_State *L);
static int luaC_lunar_zeros(lua_State *L);
static int luaC_lunar_dtype(lua_State *L);


static int luaC_lunar_sin(lua_State *L);
static int luaC_lunar_cos(lua_State *L);
static int luaC_lunar_tan(lua_State *L);

static int luaC_lunar_sin(lua_State *L);
static int luaC_lunar_cos(lua_State *L);
static int luaC_lunar_tan(lua_State *L);

static int luaC_lunar_asin(lua_State *L);
static int luaC_lunar_acos(lua_State *L);
static int luaC_lunar_atan(lua_State *L);

static int luaC_lunar_sinh(lua_State *L);
static int luaC_lunar_cosh(lua_State *L);
static int luaC_lunar_tanh(lua_State *L);

static int luaC_lunar_asinh(lua_State *L);
static int luaC_lunar_acosh(lua_State *L);
static int luaC_lunar_atanh(lua_State *L);

static int luaC_lunar_exp(lua_State *L);
static int luaC_lunar_log(lua_State *L);
static int luaC_lunar_log10(lua_State *L);


static int luaC_array__tostring(lua_State *L);
static int luaC_array__len(lua_State *L);
static int luaC_array__index(lua_State *L);
static int luaC_array__newindex(lua_State *L);
static int luaC_array__add(lua_State *L);
static int luaC_array__sub(lua_State *L);
static int luaC_array__mul(lua_State *L);
static int luaC_array__div(lua_State *L);
static int luaC_array__pow(lua_State *L);
static int luaC_array__gc(lua_State *L);


static int luaC_complex__tostring(lua_State *L);
static int luaC_complex__add(lua_State *L);
static int luaC_complex__sub(lua_State *L);
static int luaC_complex__mul(lua_State *L);
static int luaC_complex__div(lua_State *L);
static int luaC_complex__pow(lua_State *L);


static int   _array_binary_op1(lua_State *L, enum ArrayOperation op);
static int   _array_binary_op2(lua_State *L, enum ArrayOperation op);

static int _complex_binary_op1(lua_State *L, enum ArrayOperation op);
static int _complex_binary_op2(lua_State *L, enum ArrayOperation op);

static void _unary_func(lua_State *L, double(*f)(double), Complex(*g)(Complex));

static Complex ImaginaryUnit = I;




int luaopen_lunar(lua_State *L)
{
  lua_settop(L, 0); // start with an empty stack

  // Create the 'array' metatable
  // ---------------------------------------------------------------------------
  luaL_newmetatable(L, "array");
  LUA_NEW_METAMETHOD(L, array, tostring);
  LUA_NEW_METAMETHOD(L, array, len);
  LUA_NEW_METAMETHOD(L, array, index);
  LUA_NEW_METAMETHOD(L, array, newindex);
  LUA_NEW_METAMETHOD(L, array, add);
  LUA_NEW_METAMETHOD(L, array, sub);
  LUA_NEW_METAMETHOD(L, array, mul);
  LUA_NEW_METAMETHOD(L, array, div);
  LUA_NEW_METAMETHOD(L, array, pow);
  LUA_NEW_METAMETHOD(L, array, gc);
  lua_pop(L, 1);


  // Create the 'complex' metatable
  // ---------------------------------------------------------------------------
  luaL_newmetatable(L, "complex");
  LUA_NEW_METAMETHOD(L, complex, tostring);
  LUA_NEW_METAMETHOD(L, complex, add);
  LUA_NEW_METAMETHOD(L, complex, sub);
  LUA_NEW_METAMETHOD(L, complex, mul);
  LUA_NEW_METAMETHOD(L, complex, div);
  LUA_NEW_METAMETHOD(L, complex, pow);
  lua_pop(L, 1);


  // Create the 'lunar' table
  // ---------------------------------------------------------------------------
  lua_newtable(L);
  LUA_NEW_MODULEMETHOD(L, lunar, table);
  LUA_NEW_MODULEMETHOD(L, lunar, array);
  LUA_NEW_MODULEMETHOD(L, lunar, zeros);
  LUA_NEW_MODULEMETHOD(L, lunar, dtype);

  LUA_NEW_MODULEMETHOD(L, lunar, sin);
  LUA_NEW_MODULEMETHOD(L, lunar, cos);
  LUA_NEW_MODULEMETHOD(L, lunar, tan);

  LUA_NEW_MODULEMETHOD(L, lunar, asin);
  LUA_NEW_MODULEMETHOD(L, lunar, acos);
  LUA_NEW_MODULEMETHOD(L, lunar, atan);

  LUA_NEW_MODULEMETHOD(L, lunar, sinh);
  LUA_NEW_MODULEMETHOD(L, lunar, cosh);
  LUA_NEW_MODULEMETHOD(L, lunar, tanh);

  LUA_NEW_MODULEMETHOD(L, lunar, asinh);
  LUA_NEW_MODULEMETHOD(L, lunar, acosh);
  LUA_NEW_MODULEMETHOD(L, lunar, atanh);

  LUA_NEW_MODULEMETHOD(L, lunar, exp);
  LUA_NEW_MODULEMETHOD(L, lunar, log);
  LUA_NEW_MODULEMETHOD(L, lunar, log10);

  LUA_NEW_MODULEDATA(L, ARRAY_TYPE_CHAR   , char);
  LUA_NEW_MODULEDATA(L, ARRAY_TYPE_SHORT  , short);
  LUA_NEW_MODULEDATA(L, ARRAY_TYPE_INT    , int);
  LUA_NEW_MODULEDATA(L, ARRAY_TYPE_LONG   , long);
  LUA_NEW_MODULEDATA(L, ARRAY_TYPE_FLOAT  , float);
  LUA_NEW_MODULEDATA(L, ARRAY_TYPE_DOUBLE , double);
  LUA_NEW_MODULEDATA(L, ARRAY_TYPE_COMPLEX, complex);

  // Register the purely imaginary number 'I'
  lua_pushlightuserdata(L, &ImaginaryUnit);
  luaL_getmetatable(L, "complex");
  lua_setmetatable(L, -2);
  lua_setfield(L, 1, "I");

  lua_setglobal(L, "lunar");


  return 0;
}




// *****************************************************************************
// Implementation of lunar.complex metatable
//
// *****************************************************************************
int luaC_array__gc(lua_State *L)
{
  struct Array *A = (struct Array*) luaL_checkudata(L, 1, "array");
  array_del(A);
  return 0;
}

int luaC_array__tostring(lua_State *L)
{
  struct Array *A = (struct Array*) luaL_checkudata(L, 1, "array");

  lua_pushstring(L, "  [ ");
  int nstr = 1;
  for (int n=0; n<A->size; ++n) {

    char s[64];

    switch (A->type) {
    case ARRAY_TYPE_CHAR    : sprintf(s, "%d" , ((char   *)A->data)[n]); break;
    case ARRAY_TYPE_SHORT   : sprintf(s, "%d" , ((short  *)A->data)[n]); break;
    case ARRAY_TYPE_INT     : sprintf(s, "%d" , ((int    *)A->data)[n]); break;
    case ARRAY_TYPE_LONG    : sprintf(s, "%ld", ((long   *)A->data)[n]); break;
    case ARRAY_TYPE_FLOAT   : sprintf(s, "%g" , ((float  *)A->data)[n]); break;
    case ARRAY_TYPE_DOUBLE  : sprintf(s, "%g" , ((double *)A->data)[n]); break;
    case ARRAY_TYPE_COMPLEX : sprintf(s, "%g%s%gj",
				      creal(((Complex*)A->data)[n]),
				      cimag(((Complex*)A->data)[n]) > 0.0 ? "+" : "-",
				      fabs(cimag(((Complex*)A->data)[n]))); break;
    }

    if (n == A->size-1) {
      lua_pushfstring(L, "%s", s);
    }
    else {
      lua_pushfstring(L, "%s, ", s);
    }
    if ((n+1) % 10 == 0 && n != 0 && n != A->size-1) {
      lua_pushstring(L, "\n    "); ++nstr;
    }
  }
  lua_pushstring(L, " ]"); ++nstr;
  lua_concat(L, A->size + nstr);

  return 1;
}

int luaC_array__len(lua_State *L)
{
  struct Array *A = (struct Array*) luaL_checkudata(L, 1, "array");
  lua_pushnumber(L, A->size);
  return 1;
}

int luaC_array__index(lua_State *L)
{
  struct Array *A = (struct Array*) luaL_checkudata(L, 1, "array");
  const int n = luaL_checkinteger(L, 2);

  if (n >= A->size) {
    luaL_error(L, "index %d out of bounds on array of length %d", n, A->size);
  }

  switch (A->type) {
  case ARRAY_TYPE_CHAR    : lua_pushnumber(L,    ((char   *)A->data)[n]); break;
  case ARRAY_TYPE_SHORT   : lua_pushnumber(L,    ((short  *)A->data)[n]); break;
  case ARRAY_TYPE_INT     : lua_pushnumber(L,    ((int    *)A->data)[n]); break;
  case ARRAY_TYPE_LONG    : lua_pushnumber(L,    ((long   *)A->data)[n]); break;
  case ARRAY_TYPE_FLOAT   : lua_pushnumber(L,    ((float  *)A->data)[n]); break;
  case ARRAY_TYPE_DOUBLE  : lua_pushnumber(L,    ((double *)A->data)[n]); break;
  case ARRAY_TYPE_COMPLEX : lunar_pushcomplex(L, ((Complex*)A->data)[n]); break;
  }
  return 1;
}

int luaC_array__newindex(lua_State *L)
{
  struct Array *A = (struct Array*) luaL_checkudata(L, 1, "array");
  const int n = luaL_checkinteger(L, 2);
  const enum ArrayType T = A->type;

  if (n >= A->size) {
    luaL_error(L, "index %d out of bounds on array of length %d", n, A->size);
  }

  void *val = lunar_tovalue(L, T);
  memcpy((char*)A->data + array_sizeof(T)*n, val, array_sizeof(T));
  free(val);

  return 0;
}

int luaC_array__add(lua_State *L) { return _array_binary_op1(L, ARRAY_OP_ADD); }
int luaC_array__sub(lua_State *L) { return _array_binary_op1(L, ARRAY_OP_SUB); }
int luaC_array__mul(lua_State *L) { return _array_binary_op1(L, ARRAY_OP_MUL); }
int luaC_array__div(lua_State *L) { return _array_binary_op1(L, ARRAY_OP_DIV); }
int luaC_array__pow(lua_State *L) { return _array_binary_op1(L, ARRAY_OP_POW); }


int _array_binary_op1(lua_State *L, enum ArrayOperation op)
{
  if (!lunar_hasmetatable(L, 1, "array")) {
    struct Array *B = (struct Array*) luaL_checkudata(L, 2, "array");
    lunar_upcast(L, 1, B->type, B->size);
    lua_replace(L, 1);
  }
  if (!lunar_hasmetatable(L, 2, "array")) {
    struct Array *A = (struct Array*) luaL_checkudata(L, 1, "array");
    lunar_upcast(L, 2, A->type, A->size);
    lua_replace(L, 2);
  }

  return _array_binary_op2(L, op);
}

int _array_binary_op2(lua_State *L, enum ArrayOperation op)
{
  struct Array *A = (struct Array*) luaL_checkudata(L, 1, "array");
  struct Array *B = (struct Array*) luaL_checkudata(L, 2, "array");

  if (A->size != B->size) {
    luaL_error(L, "arrays could not be broadcast together with shapes (%d) (%d)",
	       A->size, B->size);
  }
  const int N = A->size;
  enum ArrayType T = (A->type >= B->type) ? A->type : B->type;

  struct Array A_ = (A->type == T) ? *A : array_new_copy(A, T);
  struct Array B_ = (B->type == T) ? *B : array_new_copy(B, T);

  struct Array *C = (struct Array*) lua_newuserdata(L, sizeof(struct Array));
  *C = array_new_zeros(N, T);

  array_binary_op(&A_, &B_, C, op);

  luaL_getmetatable(L, "array");
  lua_setmetatable(L, -2);

  if (A->type != T) array_del(&A_);
  if (B->type != T) array_del(&B_);

  return 1;
}






// *****************************************************************************
// Implementation of lunar.complex metatable
//
// *****************************************************************************
int luaC_complex__tostring(lua_State *L)
{
  Complex z = *((Complex*) luaL_checkudata(L, 1, "complex"));

  lua_pushfstring(L, "%f%s%fj", creal(z), cimag(z)>0.0?"+":"-", fabs(cimag(z)));
  return 1;
}
int luaC_complex__add(lua_State *L) { return _complex_binary_op1(L, ARRAY_OP_ADD); }
int luaC_complex__sub(lua_State *L) { return _complex_binary_op1(L, ARRAY_OP_SUB); }
int luaC_complex__mul(lua_State *L) { return _complex_binary_op1(L, ARRAY_OP_MUL); }
int luaC_complex__div(lua_State *L) { return _complex_binary_op1(L, ARRAY_OP_DIV); }
int luaC_complex__pow(lua_State *L) { return _complex_binary_op1(L, ARRAY_OP_POW); }


int _complex_binary_op1(lua_State *L, enum ArrayOperation op)
{
  if (!lua_isuserdata(L, 1)) {
    double a = lua_tonumber(L, 1);
    Complex *v = (Complex*) lua_newuserdata(L, sizeof(Complex));
    luaL_getmetatable(L, "complex");
    lua_setmetatable(L, -2);

    *v = a;
    lua_replace(L, 1);
  }

  if (!lua_isuserdata(L, 2)) {
    double a = lua_tonumber(L, 2);
    Complex *w = (Complex*) lua_newuserdata(L, sizeof(Complex));
    luaL_getmetatable(L, "complex");
    lua_setmetatable(L, -2);

    *w = a;
    lua_replace(L, 2);
  }

  return _complex_binary_op2(L, op);
}

int _complex_binary_op2(lua_State *L, enum ArrayOperation op)
{
  Complex v = *((Complex*) luaL_checkudata(L, 1, "complex"));
  Complex w = *((Complex*) luaL_checkudata(L, 2, "complex"));

  Complex *z = (Complex*) lua_newuserdata(L, sizeof(Complex));
  luaL_getmetatable(L, "complex");
  lua_setmetatable(L, -2);

  switch (op) {
  case ARRAY_OP_ADD: *z = v + w; break;
  case ARRAY_OP_SUB: *z = v - w; break;
  case ARRAY_OP_MUL: *z = v * w; break;
  case ARRAY_OP_DIV: *z = v / w; break;
  case ARRAY_OP_POW: *z = cpow(v,w); break;
  }

  return 1;
}








int luaC_lunar_table(lua_State *L)
{
  lunar_totable(L, 1);
  return 1;
}

int luaC_lunar_array(lua_State *L)
{
  const enum ArrayType T = (enum ArrayType) luaL_optinteger(L, 2, ARRAY_TYPE_DOUBLE);
  lunar_upcast(L, 1, T, 1);
  return 1;
}

int luaC_lunar_zeros(lua_State *L)
{
  const int N = luaL_checkinteger(L, 1);
  const enum ArrayType T = (enum ArrayType) luaL_optinteger(L, 2, ARRAY_TYPE_DOUBLE);
  struct Array A = array_new_zeros(N, T);
  lunar_pusharray1(L, &A);
  return 1;
}

int luaC_lunar_dtype(lua_State *L)
{
  struct Array *A = (struct Array*) luaL_checkudata(L, 1, "array");
  lua_pushstring(L, array_typename(A->type));
  return 1;
}



int luaC_lunar_sin(lua_State *L) { _unary_func(L, sin, csin); return 1; }
int luaC_lunar_cos(lua_State *L) { _unary_func(L, cos, ccos); return 1; }
int luaC_lunar_tan(lua_State *L) { _unary_func(L, tan, ctan); return 1; }

int luaC_lunar_asin(lua_State *L) { _unary_func(L, asin, casin); return 1; }
int luaC_lunar_acos(lua_State *L) { _unary_func(L, acos, cacos); return 1; }
int luaC_lunar_atan(lua_State *L) { _unary_func(L, atan, catan); return 1; }

int luaC_lunar_sinh(lua_State *L) { _unary_func(L, sinh, csinh); return 1; }
int luaC_lunar_cosh(lua_State *L) { _unary_func(L, cosh, ccosh); return 1; }
int luaC_lunar_tanh(lua_State *L) { _unary_func(L, tanh, ctanh); return 1; }

int luaC_lunar_asinh(lua_State *L) { _unary_func(L, asinh, casinh); return 1; }
int luaC_lunar_acosh(lua_State *L) { _unary_func(L, acosh, cacosh); return 1; }
int luaC_lunar_atanh(lua_State *L) { _unary_func(L, atanh, catanh); return 1; }

int luaC_lunar_exp(lua_State *L) { _unary_func(L, exp, cexp); return 1; }
int luaC_lunar_log(lua_State *L) { _unary_func(L, log, clog); return 1; }
int luaC_lunar_log10(lua_State *L) { _unary_func(L, log10, NULL); return 1; }


void _unary_func(lua_State *L, double(*f)(double), Complex(*g)(Complex))
{
  if (lua_isnumber(L, 1)) {
    const double x = lua_tonumber(L, 1);
    lua_pushnumber(L, f(x));
  }
  else if (lunar_hasmetatable(L, 1, "complex")) {

    if (g == NULL) {
      luaL_error(L, "complex operation not supported\n");
    }

    const Complex z = lunar_checkcomplex(L, 1);
    lunar_pushcomplex(L, g(z));
  }
  else if (lunar_hasmetatable(L, 1, "array")) {
    struct Array *A = (struct Array*) lua_touserdata(L, 1);

    if (A->type <= ARRAY_TYPE_DOUBLE) {
      struct Array B = array_new_copy(A, ARRAY_TYPE_DOUBLE);
      double *b = (double*) B.data;
      for (int i=0; i<B.size; ++i) b[i] = f(b[i]);
      lunar_pusharray1(L, &B);
    }
    else if (A->type == ARRAY_TYPE_COMPLEX) {

      if (g == NULL) {
	luaL_error(L, "complex operation not supported\n");
      }

      struct Array B = array_new_copy(A, ARRAY_TYPE_COMPLEX);
      Complex *b = (Complex*) B.data;
      for (int i=0; i<B.size; ++i) b[i] = g(b[i]);
      lunar_pusharray1(L, &B);
    }
  }
}
