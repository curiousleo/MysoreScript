#include <string.h>
#include "parser.hh"

using namespace AST;
using namespace MysoreScript;

namespace {
/**
 * Indicates whether a particular object needs to be visible to the GC.  If
 * it's a real pointer, then it does, otherwise (if it's an integer hidden in
 * the pointer value), then it doesn't.
 */
inline bool needsGC(Obj o)
{
	return (o != nullptr) && (((intptr_t)o & 7) == 0);
}

Interpreter::Context *currentContext;

using MysoreScript::Closure;
/**
 * 0-argument trampoline for jumping back into the interpreter when a closure
 * that has not yet been compiled is executed.
 */
Obj closureTrampoline0(Closure *C)
{
	return C->AST->interpretClosure(*currentContext, C, nullptr);
}
/**
 * 1-argument trampoline for jumping back into the interpreter when a closure
 * that has not yet been compiled is executed.
 */
Obj closureTrampoline1(Closure *C, Obj o0)
{
	Obj args[] = { o0 };
	return C->AST->interpretClosure(*currentContext, C, args);
}
/**
 * 2-argument trampoline for jumping back into the interpreter when a closure
 * that has not yet been compiled is executed.
 */
Obj closureTrampoline2(Closure *C, Obj o0, Obj o1)
{
	Obj args[] = { o0, o1 };
	return C->AST->interpretClosure(*currentContext, C, args);
}
/**
 * 3-argument trampoline for jumping back into the interpreter when a closure
 * that has not yet been compiled is executed.
 */
Obj closureTrampoline3(Closure *C, Obj o0, Obj o1, Obj o2)
{
	Obj args[] = { o0, o1, o2 };
	return C->AST->interpretClosure(*currentContext, C, args);
}
/**
 * 4-argument trampoline for jumping back into the interpreter when a closure
 * that has not yet been compiled is executed.
 */
Obj closureTrampoline4(Closure *C, Obj o0, Obj o1, Obj o2, Obj o3)
{
	Obj args[] = { o0, o1, o2, o3 };
	return C->AST->interpretClosure(*currentContext, C, args);
}
/**
 * 5-argument trampoline for jumping back into the interpreter when a closure
 * that has not yet been compiled is executed.
 */
Obj closureTrampoline5(Closure *C, Obj o0, Obj o1, Obj o2, Obj o3, Obj o4)
{
	Obj args[] = { o0, o1, o2, o3, o4 };
	return C->AST->interpretClosure(*currentContext, C, args);
}
/**
 * 6-argument trampoline for jumping back into the interpreter when a closure
 * that has not yet been compiled is executed.
 */
Obj closureTrampoline6(Closure *C, Obj o0, Obj o1, Obj o2, Obj o3, Obj o4,
		Obj o5)
{
	Obj args[] = { o0, o1, o2, o3, o4, o5 };
	return C->AST->interpretClosure(*currentContext, C, args);
}
/**
 * 7-argument trampoline for jumping back into the interpreter when a closure
 * that has not yet been compiled is executed.
 */
Obj closureTrampoline7(Closure *C, Obj o0, Obj o1, Obj o2, Obj o3, Obj o4,
		Obj o5, Obj o6)
{
	Obj args[] = { o0, o1, o2, o3, o4, o5, o6 };
	return C->AST->interpretClosure(*currentContext, C, args);
}
/**
 * 8-argument trampoline for jumping back into the interpreter when a closure
 * that has not yet been compiled is executed.
 */
Obj closureTrampoline8(Closure *C, Obj o0, Obj o1, Obj o2, Obj o3, Obj o4,
		Obj o5, Obj o6, Obj o7)
{
	Obj args[] = { o0, o1, o2, o3, o4, o5, o6, o7 };
	return C->AST->interpretClosure(*currentContext, C, args);
}
/**
 * 9-argument trampoline for jumping back into the interpreter when a closure
 * that has not yet been compiled is executed.
 */
Obj closureTrampoline9(Closure *C, Obj o0, Obj o1, Obj o2, Obj o3, Obj o4,
		Obj o5, Obj o6, Obj o7, Obj o8)
{
	Obj args[] = { o0, o1, o2, o3, o4, o5, o6, o7, o8 };
	return C->AST->interpretClosure(*currentContext, C, args);
}
/**
 * 10-argument trampoline for jumping back into the interpreter when a closure
 * that has not yet been compiled is executed.
 */
Obj closureTrampoline10(Closure *C, Obj o0, Obj o1, Obj o2, Obj o3, Obj o4,
		Obj o5, Obj o6, Obj o7, Obj o8, Obj o9)
{
	Obj args[] = { o0, o1, o2, o3, o4, o5, o6, o7, o8, o9 };
	return C->AST->interpretClosure(*currentContext, C, args);
}

/**
 * 0-argument trampoline for jumping back into the interpreter when a method
 * that has not yet been compiled is executed.
 */
Obj methodTrampoline0(Obj self, Selector cmd)
{
	Class *cls = isInteger(self) ? &SmallIntClass : self->isa;
	Method *mth = methodForSelector(cls, cmd);
	return mth->AST->interpretMethod(*currentContext, mth, self, cmd, nullptr);
}
/**
 * 1-argument trampoline for jumping back into the interpreter when a method
 * that has not yet been compiled is executed.
 */
Obj methodTrampoline1(Obj self, Selector cmd, Obj o0)
{
	Obj args[] = { o0 };
	Class *cls = isInteger(self) ? &SmallIntClass : self->isa;
	Method *mth = methodForSelector(cls, cmd);
	return mth->AST->interpretMethod(*currentContext, mth, self, cmd, args);
}
/**
 * 2-argument trampoline for jumping back into the interpreter when a method
 * that has not yet been compiled is executed.
 */
Obj methodTrampoline2(Obj self, Selector cmd, Obj o0, Obj o1)
{
	Obj args[] = { o0, o1 };
	Class *cls = isInteger(self) ? &SmallIntClass : self->isa;
	Method *mth = methodForSelector(cls, cmd);
	return mth->AST->interpretMethod(*currentContext, mth, self, cmd, args);
}
/**
 * 3-argument trampoline for jumping back into the interpreter when a method
 * that has not yet been compiled is executed.
 */
Obj methodTrampoline3(Obj self, Selector cmd, Obj o0, Obj o1, Obj o2)
{
	Obj args[] = { o0, o1, o2 };
	Class *cls = isInteger(self) ? &SmallIntClass : self->isa;
	Method *mth = methodForSelector(cls, cmd);
	return mth->AST->interpretMethod(*currentContext, mth, self, cmd, args);
}
/**
 * 4-argument trampoline for jumping back into the interpreter when a method
 * that has not yet been compiled is executed.
 */
Obj methodTrampoline4(Obj self, Selector cmd, Obj o0, Obj o1, Obj o2, Obj o3)
{
	Obj args[] = { o0, o1, o2, o3 };
	Class *cls = isInteger(self) ? &SmallIntClass : self->isa;
	Method *mth = methodForSelector(cls, cmd);
	return mth->AST->interpretMethod(*currentContext, mth, self, cmd, args);
}
/**
 * 5-argument trampoline for jumping back into the interpreter when a method
 * that has not yet been compiled is executed.
 */
Obj methodTrampoline5(Obj self, Selector cmd, Obj o0, Obj o1, Obj o2, Obj o3,
		Obj o4)
{
	Obj args[] = { o0, o1, o2, o3, o4 };
	Class *cls = isInteger(self) ? &SmallIntClass : self->isa;
	Method *mth = methodForSelector(cls, cmd);
	return mth->AST->interpretMethod(*currentContext, mth, self, cmd, args);
}
/**
 * 6-argument trampoline for jumping back into the interpreter when a method
 * that has not yet been compiled is executed.
 */
Obj methodTrampoline6(Obj self, Selector cmd, Obj o0, Obj o1, Obj o2, Obj o3,
		Obj o4, Obj o5)
{
	Obj args[] = { o0, o1, o2, o3, o4, o5 };
	Class *cls = isInteger(self) ? &SmallIntClass : self->isa;
	Method *mth = methodForSelector(cls, cmd);
	return mth->AST->interpretMethod(*currentContext, mth, self, cmd, args);
}
/**
 * 7-argument trampoline for jumping back into the interpreter when a method
 * that has not yet been compiled is executed.
 */
Obj methodTrampoline7(Obj self, Selector cmd, Obj o0, Obj o1, Obj o2, Obj o3,
		Obj o4, Obj o5, Obj o6)
{
	Obj args[] = { o0, o1, o2, o3, o4, o5, o6 };
	Class *cls = isInteger(self) ? &SmallIntClass : self->isa;
	Method *mth = methodForSelector(cls, cmd);
	return mth->AST->interpretMethod(*currentContext, mth, self, cmd, args);
}
/**
 * 8-argument trampoline for jumping back into the interpreter when a method
 * that has not yet been compiled is executed.
 */
Obj methodTrampoline8(Obj self, Selector cmd, Obj o0, Obj o1, Obj o2, Obj o3, Obj o4,
		Obj o5, Obj o6, Obj o7)
{
	Obj args[] = { o0, o1, o2, o3, o4, o5, o6, o7 };
	Class *cls = isInteger(self) ? &SmallIntClass : self->isa;
	Method *mth = methodForSelector(cls, cmd);
	return mth->AST->interpretMethod(*currentContext, mth, self, cmd, args);
}
/**
 * 9-argument trampoline for jumping back into the interpreter when a method
 * that has not yet been compiled is executed.
 */
Obj methodTrampoline9(Obj self, Selector cmd, Obj o0, Obj o1, Obj o2, Obj o3,
		Obj o4, Obj o5, Obj o6, Obj o7, Obj o8)
{
	Obj args[] = { o0, o1, o2, o3, o4, o5, o6, o7, o8 };
	Class *cls = isInteger(self) ? &SmallIntClass : self->isa;
	Method *mth = methodForSelector(cls, cmd);
	return mth->AST->interpretMethod(*currentContext, mth, self, cmd, args);
}
/**
 * 10-argument trampoline for jumping back into the interpreter when a method
 * that has not yet been compiled is executed.
 */
Obj methodTrampoline10(Obj self, Selector cmd, Obj o0, Obj o1, Obj o2, Obj o3,
		Obj o4, Obj o5, Obj o6, Obj o7, Obj o8, Obj o9)
{
	Obj args[] = { o0, o1, o2, o3, o4, o5, o6, o7, o8, o9 };
	Class *cls = isInteger(self) ? &SmallIntClass : self->isa;
	Method *mth = methodForSelector(cls, cmd);
	return mth->AST->interpretMethod(*currentContext, mth, self, cmd, args);
}
/**
 * Array of trampolines, indexed by number or arguments.  
 */
CompiledMethod methodTrampolines[] = {
	(CompiledMethod)methodTrampoline0,
	(CompiledMethod)methodTrampoline1,
	(CompiledMethod)methodTrampoline2,
	(CompiledMethod)methodTrampoline3,
	(CompiledMethod)methodTrampoline4,
	(CompiledMethod)methodTrampoline5,
	(CompiledMethod)methodTrampoline6,
	(CompiledMethod)methodTrampoline7,
	(CompiledMethod)methodTrampoline8,
	(CompiledMethod)methodTrampoline9,
	(CompiledMethod)methodTrampoline10
};
} // end anonymous namespace

namespace Interpreter
{
/**
 * Array of trampolines, indexed by number or arguments.  
 */
ClosureInvoke closureTrampolines[] = {
	(ClosureInvoke)closureTrampoline0,
	(ClosureInvoke)closureTrampoline1,
	(ClosureInvoke)closureTrampoline2,
	(ClosureInvoke)closureTrampoline3,
	(ClosureInvoke)closureTrampoline4,
	(ClosureInvoke)closureTrampoline5,
	(ClosureInvoke)closureTrampoline6,
	(ClosureInvoke)closureTrampoline7,
	(ClosureInvoke)closureTrampoline8,
	(ClosureInvoke)closureTrampoline9,
	(ClosureInvoke)closureTrampoline10
};
void Value::set(Obj o)
{
	if (needsGC(object) && !needsGC(o))
	{
		assert(holder != nullptr);
		GC_free(holder);
		holder = nullptr;
	}
	else if (!needsGC(object) && needsGC(o))
	{
		assert(holder == nullptr);
		holder = (Obj*)GC_malloc_uncollectable(sizeof(Obj));
		*holder = o;
	}
	else if (needsGC(object) && needsGC(o))
	{
		*holder = o;
	}
	object = o;
}
Value::~Value()
{
	if (needsGC(object))
	{
		assert(holder != nullptr);
		GC_free(holder);
	}
}
Obj *Context::lookupSymbol(const std::string &name)
{
	// If there's a symbol table stack, then look in the top one.  We don't need
	// to go deeper, because any bound variables should be added automatically
	// to the top.
	if (!symbols.empty())
	{
		auto top = symbols.back();
		auto I = top->find(name);
		if (I != top->end())
		{
			return I->second;
		}
	}
	// If it's not local, then look in the global symbol table.
	auto I = globalSymbols.find(name);
	if (I != globalSymbols.end())
	{
		return I->second;
	}
	// It's not found, so return null.
	return nullptr;
}

void Context::setSymbol(const std::string &name, Obj val)
{
	Obj *addr = lookupSymbol(name);
	// If storage doesn't exist for this symbol, it's a global so
	// allocate some storage for it.
	if (!addr)
	{
		// Construct a new Value to gold the object.
		globals.emplace_front(val);
		// Get the address of the storage that we've allocated and store it in
		// the symbol table
		addr = globals.front().address();
		globalSymbols[name] = addr;
	}
	else
	{
		*addr = val;
	}
}

void Context::setSymbol(const std::string &name, Obj *val)
{
	(*symbols.back())[name] = val;
}

}

////////////////////////////////////////////////////////////////////////////////
// Interpreter methods on AST classes
////////////////////////////////////////////////////////////////////////////////

void Statements::interpret(Interpreter::Context &c)
{
	for (auto &s : statements)
	{
		// If we've executed a return statement, then stop executing.
		if (c.isReturning)
		{
			return;
		}
		s->interpret(c);
	}
}

void Statements::collectVarUses(std::unordered_set<std::string> &decls,
                                std::unordered_set<std::string> &uses)
{
	for (auto &s : statements)
	{
		s->collectVarUses(decls, uses);
	}
}

Obj Expression::evaluate(Interpreter::Context &c)
{
	// If this is a constant expression and we've cached the value, then return
	// the cached result.
	if (cache)
	{
		return cache;
	}
	// Ask the subclass to evaluate the expression
	Obj r = evaluateExpr(c);
	// Cache it, if it's a constant expression
	if (isConstantExpression())
	{
		cache = r;
	}
	return r;
}

Obj Call::evaluateExpr(Interpreter::Context &c)
{
	// Array of arguments.  
	Obj args[10];
	// Get the callee, which is either a closure or some other object that will
	// have a method on it invoked.
	Obj obj = callee->evaluate(c);
	assert(obj);
	auto &argsAST = arguments->arguments;
	size_t i=0;
	// Evaluate each argument, in order, and pop them in the array.
	for (auto &Arg : argsAST)
	{
		assert(i<(sizeof(args)/sizeof(Obj)));
		args[i++] = Arg->evaluate(c);
	}
	// Get the class
	currentContext = &c;
	// If there's no method, then we're trying to invoke a closure.
	if (!method)
	{
		assert(obj->isa == &ClosureClass);
		Closure *closure = (Closure*)obj;
		return callCompiledClosure(closure->invoke, closure, args, i);
	}
	// Look up the selector and method to call
	Selector sel = lookupSelector(method->name);
	assert(sel);
	CompiledMethod mth = compiledMethodForSelector(obj, sel);
	assert(mth);
	// Call the method.
	return callCompiledMethod(mth, obj, sel, args, arguments->arguments.size());
}

Obj VarRef::evaluateExpr(Interpreter::Context &c)
{
	// Get the address of the variable corresponding to this symbol and then
	// load the object stored there.
	return *c.lookupSymbol(name->name);
}

void ClosureDecl::check()
{
	// Don't do this if we've already done it.
	if (checked)
	{
		return;
	}
	// Recursively collect all of the variables that are declared and referenced
	// by statements in this closure.
	body->collectVarUses(decls, boundVars);
	// Parameters are not bound variables, they're explicitly passed in.
	for (auto &param : parameters->arguments.objects())
	{
		boundVars.erase(param->name);
	}
	// Variables that are declared in this function are also not bound
	// variables.
	for (auto &decl : decls)
	{
		boundVars.erase(decl);
	}
	checked = true;
}

void ClosureDecl::collectVarUses(std::unordered_set<std::string> &decls,
                                 std::unordered_set<std::string> &uses)
{
	// Find all of the variables that are used by this closure.
	check();
	// Add any bound variables to the use list
	uses.insert(boundVars.begin(), boundVars.end());
	// Add the name of this closure to the declared list in the enclosing scope.
	decls.insert(name->name);
}

Obj ClosureDecl::evaluateExpr(Interpreter::Context &c)
{
	check();
	// Allocate the closure, with enough space for all of the bound variables
	// at the end.
	// Note that, in the current implementation, closures are always allocated
	// on the heap.  In many cases, closures do not persist longer than their
	// wrapping function.  In these cases, we could potentially allocate them
	// on the stack, but making this determination requires a write barrier for
	// the store.
	Closure *C = gcAlloc<Closure>(boundVars.size() * sizeof(Obj));
	// Set up the class pointer
	C->isa = &ClosureClass;
	// Set up the parameter count
	size_t params = parameters->arguments.objects().size();
	C->parameters = createSmallInteger(params);
	C->AST = this;
	C->invoke = compiledClosure ? compiledClosure :
		Interpreter::closureTrampolines[params];
	c.setSymbol(name->name, (Obj)C);
	int i=0;
	// Copy bound variables into the closure.
	for (auto &var : boundVars)
	{
		C->boundVars[i++] = *c.lookupSymbol(var);
	}
	return (Obj)C;
}

Obj ClosureDecl::interpretMethod(Interpreter::Context &c, Method *mth, Obj self,
		Selector sel, Obj *args)
{
	check();
	Class *cls = isInteger(self) ? &SmallIntClass : self->isa;
	executionCount++;
	// If we've interpreted this method enough times then try to compile it.
	if (executionCount == compileThreshold)
	{
		mth->function = compileMethod(cls, c.globalSymbols);
		compiledClosure = (ClosureInvoke)mth->function;
	}
	// If we now have a compiled version, try to execute it.
	if (compiledClosure)
	{
		return callCompiledMethod((CompiledMethod)compiledClosure, self, sel,
				args, parameters->arguments.objects().size());
	}
	// Create a new symbol table for this method.
	Interpreter::SymbolTable closureSymbols;
	c.pushSymbols(closureSymbols);
	// Point the symbols for the arguments at our arguments array
	size_t i=0;
	for (auto &param : parameters->arguments)
	{
		c.setSymbol(param->name, &args[i++]);
	}
	Obj cmdObj = createSmallInteger(sel);
	// Add self and cmd (receiver and selector) to the symbol table
	c.setSymbol("self", &self);
	c.setSymbol("cmd", &cmdObj);
	// Add the addresses of the ivars in the self object to the symbol table.
	Obj *ivars = ((Obj*)(&self->isa)) + 1;
	for (int32_t i=0 ; i<cls->indexedIVarCount ; i++)
	{
		c.setSymbol(cls->indexedIVarNames[i], &ivars[i]);
	}
	// Interpret the statements in this method;
	body->interpret(c);
	// Return the return value.  Make sure it's set back to nullptr after we've
	// copied it so that we always return null from any method that doesn't
	// explicitly return.
	Obj retVal = c.retVal;
	c.retVal = nullptr;
	c.isReturning = false;
	// Pop the symbols off the symbol table (very important, as they reference
	// our stack frame!)
	c.popSymbols();
	return retVal;
}
Obj ClosureDecl::interpretClosure(Interpreter::Context &c, Closure *self,
		Obj *args)
{
	executionCount++;
	// If we've interpreted this enough times, compile it.
	if (executionCount == compileThreshold)
	{
		// Note that we don't pass any symbols other than the globals into the
		// compiler, because all of the bound variables are already copied into
		// the closure object when it is created.
		self->invoke = compileClosure(c.globalSymbols);
		compiledClosure = self->invoke;
	}
	// If we now have a compiled version, call it
	if (compiledClosure)
	{
		callCompiledClosure(compiledClosure, self, args,
				parameters->arguments.objects().size());
	}
	// Create a new symbol table for this closure
	Interpreter::SymbolTable closureSymbols;
	c.pushSymbols(closureSymbols);
	size_t i=0;
	// Add the parameters and the bound variables to the symbol table.
	for (auto &param : parameters->arguments)
	{
		// Parameters are referenced from the arguments array
		c.setSymbol(param->name, &args[i++]);
	}
	i = 0;
	for (auto &bound : boundVars)
	{
		// Bound variables are stored within the closure object
		c.setSymbol(bound, &self->boundVars[i++]);
	}
	// Interpret the body
	body->interpret(c);
	// Return the return value.  Make sure it's set back to nullptr after we've
	// copied it so that we always return null from any method that doesn't
	// explicitly return.
	Obj retVal = c.retVal;
	c.retVal = nullptr;
	c.isReturning = false;
	// Pop the symbols off the symbol table (very important, as they reference
	// our stack frame!)
	c.popSymbols();
	return retVal;
}

Obj StringLiteral::evaluateExpr(Interpreter::Context &c)
{
	// Construct a string object.
	String *str = gcAlloc<String>(value.size());
	assert(str != nullptr);
	// Set the class pointer
	str->isa = &StringClass;
	// Set the length (as a small integer)
	str->length = MysoreScript::createSmallInteger(value.size());
	// Copy the characters into the object
	value.copy(str->characters, value.size(), 0);
	return (Obj)str;
}

void IfStatement::interpret(Interpreter::Context &c)
{
	if (((intptr_t)condition->evaluate(c)) & ~7)
	{
		body->interpret(c);
	}
}
void WhileLoop::interpret(Interpreter::Context &c)
{
	while (((intptr_t)condition->evaluate(c)) & ~7)
	{
		body->interpret(c);
	}
}
void Decl::interpret(Interpreter::Context &c)
{
	// Declarations don't normally allocate space for variables, but at the
	// global scope then storing null in a global will trigger its allocation.
	Obj v = nullptr;
	if (init)
	{
		v = init->evaluate(c);
	}
	c.setSymbol(name->name, v);
}

void Assignment::interpret(Interpreter::Context &c)
{
	c.setSymbol(target->name->name, expr->evaluate(c));
}

Obj BinOp::evaluateExpr(Interpreter::Context &c)
{
	Obj LHS = lhs->evaluate(c);
	Obj RHS = rhs->evaluate(c);
	// If this is a comparison, then we're doing a pointer-compare even if
	// they're objects.  If both sides are small integers, then ask the subclass
	// to look up their integer values.
	if (isComparison() || (isInteger(LHS) && isInteger(RHS)))
	{
		// Ask the subclass to evaluate the expression with integer arguments.
		// Note that we can't use getInteger() here because the assert that this
		// really is a small integer is not valid - if we're doing a comparison
		// then the arguments might be pointers.
		return createSmallInteger(evaluateWithIntegers(
					((intptr_t)LHS) >> 3,
					((intptr_t)RHS) >> 3));
	}
	Selector sel = lookupSelector(methodName());
	CompiledMethod mth = compiledMethodForSelector(LHS, sel);
	return ((Obj(*)(Obj,Selector,Obj))mth)(LHS, sel, RHS);
}

void Return::interpret(Interpreter::Context &c)
{
	// Evaluate the returned expression and then indicate in the interpreter
	// context that we're returning.
	c.retVal = expr->evaluate(c);
	c.isReturning = true;
}

void ClassDecl::interpret(Interpreter::Context &c)
{
	// Construct the new class.  The class table persists over the lifetime of
	// the program, so memory allocated here is never freed.
	Class *cls = new Class();
	// Due to the way automatic AST construction works, we'll end up with the
	// class name in the superclass name field if we don't have a superclass.
	std::string &clsName = name ? name->name : superclassName->name;
	cls->superclass = name ? lookupClass(superclassName->name) : nullptr;
	cls->className = strdup(clsName.c_str());
	cls->methodCount = methods.size();
	cls->indexedIVarCount = ivars.size();
	// Construct the method list, with one Method structure for the metadata for
	// each method.
	cls->methodList = new Method[cls->methodCount];
	Method *method = cls->methodList;
	for (auto &m : methods)
	{
		method->selector = lookupSelector(m->name->name);
		method->args = m->parameters->arguments.size();
		// Currently, we only have trampolines for up to 10 arguments.  We could
		// reuse some of the JIT code to generate new ones at run time if
		// this were intended for production use, but this is okay as an
		// example.
		assert(method->args <= 10);
		// Insert a trampoline for the method
		method->function = methodTrampolines[method->args];
		// We retain ownership of the AST node, but the method will contain a
		// pointer to it.
		method->AST = m.get();
		method++;
	}
	// Set up the names of the instance variables.
	cls->indexedIVarNames = new const char*[cls->indexedIVarCount];
	const char **ivar = cls->indexedIVarNames;
	for (auto &i : ivars)
	{
		*(ivar++) = strdup(i->name->name.c_str());
	}
	// Add the class to the class table.
	registerClass(clsName, cls);
}
Obj NewExpr::evaluateExpr(Interpreter::Context &c)
{
	// Look up the class in the class table and create a new instance of it.
	return newObject(lookupClass(className->name));
}

