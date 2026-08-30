# OOP in C++ — Interview & LLD Question Bank

Numbered questions with answers below each one — hide/reveal manually as you like in Obsidian.

---

## 1. Basics of OOP (Classes & Objects)

1. What is Object-Oriented Programming? How does it differ from procedural programming?  
    OOP organizes code around objects that bundle data (state) and behavior (methods), using concepts like encapsulation, inheritance, and polymorphism. Procedural programming organizes code as a sequence of functions operating on shared data, with no inherent binding between data and the functions that act on it.
    
2. What is a class? What is an object?  
    A class is a user-defined blueprint/template that defines the properties (data members) and behaviors (member functions) an object will have. An object is a concrete instance of a class, occupying actual memory.
    
3. What are the four pillars of OOP?  
    Encapsulation, Abstraction, Inheritance, and Polymorphism.
    
4. What is the difference between a class and a structure in C++?  
    Functionally almost identical in C++. The only real differences: members and inheritance are `public` by default in a struct, and `private` by default in a class. By convention, structs are used for plain data holders and classes for objects with behavior.
    
5. What is data hiding, and how is it different from abstraction?  
    Data hiding is restricting direct access to an object's internal data (via `private`/`protected`) so it can only be manipulated through defined interfaces. Abstraction is about hiding implementation complexity and exposing only relevant functionality — data hiding is one mechanism used to achieve abstraction, but abstraction is a broader design-level concept.
    
6. What happens when you create an object of an empty class in C++? What is its `sizeof`?  
    The compiler still assigns it a size of 1 byte (not 0), so that two distinct objects of the empty class have distinct addresses in memory.
    
7. What is the difference between an object and an instance?  
    They are effectively the same thing — "instance" is the term used to describe a specific realization of a class, and "object" is that realization in memory. They're used interchangeably in practice.
    
8. What is the difference between a member function and a member variable?  
    A member variable stores the state/data of an object, existing per-object (unless static). A member function defines behavior/operations on that data and is shared across all objects (only one copy exists in code memory, not per-object).
    

---

## 2. Encapsulation & Access Specifiers

1. What is encapsulation? Give a real-world example.  
    Encapsulation is bundling data and the methods that operate on it into a single unit (class), while restricting direct external access to internal state. Example: a `BankAccount` class exposes `deposit()`/`withdraw()` methods but keeps the `balance` variable private, preventing arbitrary external modification.
    
2. What are the three access specifiers in C++, and what is their scope?  
    `public` — accessible from anywhere. `protected` — accessible within the class and its derived classes. `private` — accessible only within the class itself (and its friends).
    
3. What is the default access specifier for class and struct?  
    `private` for class, `public` for struct.
    
4. Can private members be accessed outside the class? How?  
    Not directly. They can be accessed indirectly via public getter/setter member functions, or directly by a `friend` function/class that has been explicitly granted access.
    
5. What happens to base class members when inheritance is public/protected/private?  
    Public inheritance keeps base public members public and protected members protected in the derived class. Protected inheritance makes both public and protected base members protected in the derived class. Private inheritance makes both public and protected base members private in the derived class. In all cases, private base members remain inaccessible directly to the derived class.
    
6. Why is encapsulation important for maintainability?  
    It lets you change the internal implementation of a class without breaking code that depends on it, as long as the public interface stays the same. It also prevents invalid states by funneling all mutations through controlled methods.
    
7. What is the practical difference between "protected" and "private" regarding inheritance?  
    A `private` member is inaccessible to any derived class, whereas a `protected` member is accessible to derived classes but still hidden from the outside world. Use protected when subclasses genuinely need direct access to a member.
    

---

## 3. Abstraction

1. What is abstraction? How is it different from encapsulation?  
    Abstraction is hiding unnecessary implementation details and exposing only the essential features/interface of an object. Encapsulation is the mechanism (bundling data + restricting access) that helps achieve abstraction — abstraction is about design/interface, encapsulation is about implementation/access control.
    
2. How is abstraction achieved in C++?  
    Through access specifiers (hiding implementation details), abstract classes and pure virtual functions (defining a contract without implementation), and header files exposing only declarations.
    
3. What is an abstract class?  
    A class that contains at least one pure virtual function (`virtual void f() = 0;`). It cannot be instantiated directly and is meant to be used as a base class defining an interface/contract for derived classes.
    
4. Can we create an object of an abstract class? Why or why not?  
    No. Since it has at least one function with no implementation (pure virtual), the compiler cannot create a complete, callable object — instantiation is disallowed at compile time.
    
5. Can an abstract class have a constructor? Why would it need one?  
    Yes. Even though it can't be instantiated directly, its constructor runs when a derived class object is created, to initialize the base part of the object (e.g., common member variables).
    
6. Explain abstraction vs encapsulation with a concrete example.  
    A car's steering wheel and pedals are the abstraction — the driver only needs to know "turn wheel to steer, press pedal to accelerate." The engine, wiring, and hydraulics hidden under the hood represent encapsulation — the internal complexity is bundled away and inaccessible directly.
    

---

## 4. Constructors

1. What is a constructor? What are its key properties?  
    A special member function automatically invoked when an object is created, used to initialize its state. Properties: same name as the class, no return type (not even void), can be overloaded, and cannot be called explicitly like a normal function.
    
2. What are the types of constructors in C++?  
    Default constructor (no parameters), parameterized constructor (takes arguments), and copy constructor (initializes an object as a copy of another object of the same class).
    
3. Can a constructor be private? What's the use case?  
    Yes. Common use cases: the Singleton design pattern (preventing external instantiation, forcing use of a static factory method), and named-constructor idioms.
    
4. Can a constructor be virtual? Why or why not?  
    No. Virtual dispatch relies on the vptr/vtable being set up, but that mechanism is established during construction itself — the object doesn't fully "exist" with its dynamic type resolved until the constructor completes. There's no meaningful way to virtually dispatch to a not-yet-existing object.
    
5. What is a copy constructor? When is it called (list at least 3 scenarios)?  
    A constructor that initializes a new object as a copy of an existing object of the same class, signature `ClassName(const ClassName &obj)`. Called when: (1) an object is initialized from another, e.g. `A b = a;`, (2) an object is passed by value to a function, (3) an object is returned by value from a function (subject to RVO/copy elision), (4) an object is used to initialize an array/container element.
    
6. What is the difference between deep copy and shallow copy?  
    A shallow copy copies member values as-is, so pointer members end up pointing to the same memory in both objects (dangerous with dynamically allocated memory — double free, dangling pointers). A deep copy allocates new memory and copies the pointed-to data, so each object owns independent memory.
    
7. What is a default constructor, and when is it auto-generated by the compiler?  
    A constructor taking no arguments (either user-defined or compiler-generated). The compiler auto-generates one only if the class defines no constructors at all.
    
8. What is constructor overloading?  
    Defining multiple constructors in a class with different parameter lists, letting objects be initialized in different ways depending on the arguments supplied.
    
9. What is the difference between using an initializer list vs assignment inside the constructor body?  
    The initializer list directly initializes members as they're constructed (one step). Assignment in the body first default-constructs the member, then assigns a new value (two steps) — less efficient, and doesn't work at all for const members, reference members, or members without a default constructor.
    
10. Why must const and reference members be initialized in the initializer list?  
    Const members and references must be bound to a value at the moment of creation and can never be reassigned afterward. The constructor body only allows assignment (which happens after the member already "exists"), so it's too late for const/references — the initializer list is the only valid place.
    
11. What is a delegating constructor (C++11)?  
    A constructor that calls another constructor of the same class in its initializer list (e.g., `ClassName() : ClassName(0) {}`), avoiding duplicated initialization logic.
    
12. What is an `explicit` constructor and why is it used?  
    A single-argument (or all-but-one-defaulted) constructor marked `explicit` cannot be used for implicit conversions — it must be called directly. This prevents unintended silent conversions, e.g., stopping `MyClass obj = 5;` from implicitly constructing an object from an int.
    

---

## 5. Destructors

1. What is a destructor? What are its key properties?  
    A special member function automatically called when an object goes out of scope or is explicitly deleted, used to release resources. Properties: same name as class prefixed with `~`, no return type, no parameters, and cannot be overloaded (only one destructor per class).
    
2. Can a destructor be overloaded?  
    No, a class can have only one destructor, since it takes no arguments and there's no way to distinguish "overloads."
    
3. What is a virtual destructor and why is it needed?  
    A destructor declared `virtual` in the base class so that deleting a derived object through a base class pointer correctly invokes the derived class's destructor first, then the base's. Needed whenever a class is meant to be used polymorphically (through base pointers) and may be deleted that way.
    
4. What happens if the base class destructor is not virtual and you delete a derived object through a base pointer?  
    Undefined behavior. Only the base class's destructor runs (since the call is resolved statically), so the derived class's members/resources are never cleaned up, causing resource/memory leaks.
    
5. What is the order of constructor and destructor calls in inheritance?  
    Constructors run base-to-derived (base class constructor first, then derived). Destructors run in the reverse order — derived-to-base (derived class destructor first, then base).
    
6. Can a destructor throw an exception? Why is it discouraged?  
    Technically yes, but it's strongly discouraged and destructors are implicitly `noexcept` by default in modern C++. If a destructor throws during stack unwinding (itself triggered by another exception), `std::terminate` is called, crashing the program.
    

---

## 6. Copy Constructor, Assignment Operator & Rule of Three/Five/Zero

1. What is the difference between a copy constructor and the copy assignment operator?  
    The copy constructor creates a brand-new object as a copy of an existing one (`A b = a;` or `A b(a);`). The copy assignment operator (`operator=`) copies the state of one already-existing object into another already-existing object (`b = a;` where `b` already exists).
    
2. When does the compiler generate a default copy constructor/assignment operator?  
    If the class doesn't declare its own, the compiler auto-generates ones that perform a member-wise (shallow) copy of each data member.
    
3. What is the Rule of Three?  
    If a class needs a custom destructor, copy constructor, or copy assignment operator, it almost certainly needs all three — typically because it manages a resource (like raw dynamic memory) that requires deep-copy semantics.
    
4. What is the Rule of Five (C++11)?  
    Extends the Rule of Three: if you need a custom destructor, copy constructor, or copy assignment, you should generally also define a move constructor and move assignment operator, to support efficient resource transfer instead of copying.
    
5. What is the Rule of Zero?  
    Design classes so they don't manage raw resources directly at all — delegate ownership to RAII types like `std::unique_ptr`, `std::vector`, `std::string`. Then you don't need to write any of the special member functions; the compiler-generated defaults just work correctly.
    
6. What is a move constructor and move assignment operator? Why were they introduced?  
    They "steal" resources (like a pointer) from a temporary/rvalue object instead of copying them, leaving the source in a valid-but-empty state. Introduced in C++11 to avoid expensive deep copies when the source object is about to be destroyed anyway (e.g., returning large objects from functions).
    
7. What is the difference between lvalue and rvalue references in the context of move semantics?  
    An lvalue reference (`T&`) binds to a named, persistent object. An rvalue reference (`T&&`) binds to temporaries/about-to-expire values, signaling to the compiler that it's safe to "move from" (steal resources from) that object rather than copy it.
    
8. What is self-assignment, and why must `operator=` handle it?  
    Self-assignment is `a = a;`. If `operator=` first frees the current object's resources and then tries to copy from the source (which is the same object), it will copy from already-freed/garbage memory. A robust `operator=` should check for self-assignment (or use the copy-and-swap idiom) to avoid this.
    

---

## 7. `this` Pointer

1. What is the `this` pointer? What is its type?  
    An implicit pointer passed to every non-static member function, pointing to the object on which the function was invoked. Inside a member function of class `A`, its type is `A* const` (a const pointer to `A`).
    
2. Can the `this` pointer be used inside a static member function? Why not?  
    No. Static member functions aren't associated with any particular object instance — they belong to the class itself — so there is no object for `this` to point to.
    
3. Why is `this` needed to return an object by reference (method chaining)?  
    Returning `*this` from a member function allows the caller to chain further method calls on the same object, e.g., `obj.setX(1).setY(2);`, since each call returns a reference to the same underlying object.
    
4. Can `this` be assigned a null value?  
    It's not meant to be reassigned — `this` is a const pointer, so you cannot modify what it points to within the function. It's technically possible for it to be null only via undefined behavior (e.g., calling a member function through a null pointer), which should never be relied upon.
    
5. What is the type of `this` inside a `const` member function?  
    `const A* const` — a const pointer to a const object, meaning you cannot modify the object's members through `this` inside that function.
    

---

## 8. Static Members

1. What is a static member variable? Where must it be defined?  
    A member variable shared by all objects of a class, existing as a single copy independent of any particular instance. It's declared inside the class but must be defined (and given storage) outside the class, at namespace scope, e.g. `int A::count = 0;` (unless it's `inline static`/`constexpr` in C++17+).
    
2. What is a static member function, and what restrictions does it have?  
    A function that belongs to the class rather than any object, callable without an instance. Restriction: it can only directly access other static members (variables/functions) since it has no `this` pointer and thus no access to non-static members.
    
3. Can static member functions access non-static members? Why not directly?  
    Not directly, because they lack a `this` pointer to identify which object's non-static members to use. They can access non-static members only if given an explicit object (e.g., passed as a parameter).
    
4. What is the difference between static and non-static member variables in memory?  
    Non-static members get a separate copy in every object's memory. Static members exist as a single shared copy in the program's static/global data segment, regardless of how many objects are created.
    
5. Can we call a static function using an object? Using the class name?  
    Yes to both — `obj.staticFunc()` works (though it's discouraged stylistically), and the canonical way is via the class name: `ClassName::staticFunc()`.
    
6. What is a static local variable inside a member function?  
    A variable local to that function's scope, but which persists its value across multiple calls (initialized only once) and is shared across all objects/calls, rather than being recreated each time.
    
7. Can a class have a static object of itself?  
    Yes — this is exactly the mechanism used in the classic Meyer's Singleton pattern, where a static local (or static member) object of the class itself is created once and returned by a static accessor method.
    

---

## 9. Friend Functions & Friend Classes

1. What is a friend function? Why do we need it?  
    A non-member function granted access to a class's private and protected members by being declared `friend` inside that class. Needed when an external function (e.g., an overloaded `operator<<`) legitimately needs deep access to a class's internals without being a member itself.
    
2. Is friendship mutual, inherited, or transitive in C++?  
    No to all three. If class A declares B as a friend, B does not automatically make A its friend (not mutual); a derived class of the friend does not inherit friendship; and if A is a friend of B and B is a friend of C, A is not automatically a friend of C (not transitive).
    
3. What is a friend class?  
    A class declared as `friend` inside another class, granting all member functions of the friend class access to the private/protected members of the class granting friendship.
    
4. Does making a function a friend violate encapsulation?  
    It technically breaks strict encapsulation since it grants outside access to internals, but it's a controlled, explicit exception — the class author decides exactly which external entities get that access, so it's considered an acceptable, deliberate design tool when used sparingly.
    
5. Can a friend function be virtual?  
    No. A friend function is not a member of the class, and virtual dispatch is a mechanism specific to member functions accessed through base class pointers/references — friend functions don't participate in a class's vtable at all.
    
6. Can you make only one member function of a class a friend of another class?  
    Yes — this is called a friend member function. You declare just that specific member function (with its full scope resolution, e.g. `friend void OtherClass::someMethod();`) as a friend, rather than granting friendship to the entire class.
    

---

## 10. Inheritance

1. What is inheritance? Why is it used?  
    A mechanism where a derived class acquires properties and behaviors of a base class, enabling code reuse and establishing an "is-a" relationship, and forming the basis for runtime polymorphism.
    
2. What are the types of inheritance in C++?  
    Single, multiple, multilevel, hierarchical, and hybrid inheritance.
    
3. What is the difference between multilevel, multiple, hierarchical, and hybrid inheritance?  
    Multilevel: A → B → C (a chain of derivation). Multiple: a class derives from more than one base class directly. Hierarchical: multiple classes derive from a single common base class. Hybrid: a combination of two or more of the above types, often what leads to the diamond problem.
    
4. What is the difference between public, protected, and private inheritance?  
    It controls how the base class's public/protected members appear in the derived class: public inheritance preserves their access levels ("is-a" relationship), protected inheritance downgrades public members to protected, and private inheritance downgrades both to private (often used to model "implemented-in-terms-of" rather than "is-a").
    
5. Can constructors be inherited?  
    Not automatically by default, but C++11 introduced inheriting constructors via a `using Base::Base;` declaration in the derived class, which brings the base class's constructors into the derived class's overload set.
    
6. How are base class constructors invoked from a derived class constructor?  
    Explicitly via the derived constructor's initializer list, e.g. `Derived(int x) : Base(x) { }`. If not specified, the base class's default constructor is called implicitly before the derived class's constructor body runs.
    
7. Can a derived class object access private members of the base class?  
    No, never directly — private members are accessible only within the base class itself (or its friends), regardless of inheritance type. The derived class can access them only indirectly through public/protected base class methods.
    
8. What is method overriding in the context of inheritance?  
    When a derived class provides its own implementation of a function that is already defined (usually as virtual) in the base class, with the same signature, so calls dispatch to the derived version at runtime.
    
9. What is the order of constructor/destructor calls in multiple inheritance?  
    Base class constructors run in the order they're listed in the derived class's declaration (not the initializer list order), followed by the derived class constructor. Destructors run in exactly the reverse order.
    
10. Why does C++ support multiple inheritance while Java doesn't (for classes)?  
    C++ gives programmers low-level control and historically prioritized flexibility, accepting the added complexity (like the diamond problem) that multiple inheritance brings. Java avoids it for classes specifically to sidestep that ambiguity/complexity, instead allowing a class to implement multiple interfaces, which don't carry state and thus avoid the diamond conflict.
    
11. What is virtual inheritance?  
    A mechanism (`class B : virtual public A`) where a derived class shares a single common instance of a repeatedly-inherited base class, rather than getting a separate copy through each inheritance path — used to solve the diamond problem.
    
12. What is composition vs inheritance, and when should you prefer one over the other?  
    Inheritance models an "is-a" relationship (tight coupling, shared interface/behavior). Composition models a "has-a" relationship (one class contains an instance of another as a member). Prefer composition when you just need to reuse functionality without wanting the full substitutability/coupling of inheritance — it's more flexible and easier to change at runtime.
    

---

## 11. Multiple Inheritance & The Diamond Problem

1. What is the diamond problem?  
    It occurs when a class D inherits from two classes B and C, which both inherit from a common base class A. Without special handling, D ends up with two separate copies of A's members (one via B, one via C), causing ambiguity when accessing them.
    
2. How does C++ resolve the diamond problem?  
    Using virtual inheritance — if B and C both inherit from A virtually (`class B : virtual public A`), then D contains only a single shared instance of A, eliminating the duplication and the resulting ambiguity.
    
3. What is a virtual base class?  
    A base class inherited virtually, ensuring that no matter how many derivation paths lead to it, only one shared instance of it exists in the most-derived object.
    
4. Without virtual inheritance, how many copies of the grandparent class exist in diamond inheritance?  
    Two — one inherited through each of the two intermediate parent classes — leading to duplicated data members and ambiguous member access.
    
5. How is ambiguity resolved when two base classes have a function with the same name (without virtual inheritance)?  
    By explicitly qualifying which base class's version to use with the scope resolution operator, e.g., `d.B::show();` or `d.C::show();`.
    
6. What are the drawbacks of multiple inheritance?  
    Increased complexity (diamond problem/ambiguity), tighter coupling between unrelated hierarchies, harder-to-maintain code, and potential for subtle bugs — which is why many modern languages (Java, C#) disallow multiple inheritance for classes and instead rely on interfaces.
    

---

## 12. Polymorphism — Compile-Time

1. What is polymorphism? What are its two types?  
    "Many forms" — the ability of the same interface/function name to behave differently depending on context. Two types: compile-time (static) polymorphism, resolved at compile time (function/operator overloading), and runtime (dynamic) polymorphism, resolved at runtime (virtual functions/overriding).
    
2. What is function overloading? What are the rules for a valid overload?  
    Defining multiple functions with the same name in the same scope but different parameter lists (different number, type, or order of parameters). The compiler picks the correct one at compile time based on the arguments passed.
    
3. What is operator overloading? Give a simple example.  
    Giving custom, class-specific meaning to an existing C++ operator. Example: overloading `+` for a `Complex` class so that `c1 + c2` adds two complex numbers using `Complex operator+(const Complex&) const;`.
    
4. Can return type alone differentiate overloaded functions?  
    No. Overload resolution is based purely on the function signature (name + parameter types), not the return type — two functions differing only in return type will cause a compile error.
    
5. What is compile-time polymorphism also known as, and why?  
    Static binding or early binding, because the compiler determines exactly which function/operator implementation to call at compile time, based on the static types and argument list, before the program even runs.
    

---

## 13. Polymorphism — Runtime, Virtual Functions

1. What is runtime polymorphism? How is it achieved in C++?  
    The ability to decide, at runtime, which overridden function implementation to call, based on the actual (dynamic) type of the object rather than the static type of the pointer/reference used. Achieved via virtual functions accessed through base class pointers or references.
    
2. What is a virtual function?  
    A member function declared with the `virtual` keyword in a base class, which can be overridden in derived classes, and whose calls through a base pointer/reference are dispatched dynamically to the most-derived override.
    
3. What is function overriding? What are the rules?  
    Redefining a base class's virtual function in a derived class with an identical signature (same name, parameters, and — with one exception — return type). Rules: the base function must be virtual, and access specifiers can be widened but the signature must otherwise match, or it becomes overloading/hiding instead.
    
4. What is the difference between overloading and overriding?  
    Overloading is having multiple functions with the same name but different parameters, resolved at compile time, typically within the same class. Overriding is redefining a base class's virtual function in a derived class with the same signature, resolved at runtime.
    
5. What is a pure virtual function? What is its syntax?  
    A virtual function with no implementation in the base class, declared as `virtual returnType funcName(params) = 0;`, forcing derived classes to provide their own implementation (unless they too remain abstract).
    
6. How does a pure virtual function relate to an abstract class?  
    A class becomes abstract the moment it contains at least one pure virtual function — this is the defining criterion for abstractness in C++, and it prevents that class from being instantiated directly.
    
7. What is early binding vs late binding?  
    Early (static) binding resolves which function to call at compile time (non-virtual calls, overloading). Late (dynamic) binding defers that decision to runtime, based on the object's actual type (virtual function calls through base pointers/references).
    
8. Why do we need virtual functions — give a motivating example?  
    Without them, calling a function through a base class pointer always invokes the base class's version regardless of the object's real type. Example: an array of `Shape*` pointing to `Circle`, `Square` objects — calling `shape->draw()` should invoke each shape's own `draw()`, which requires virtual dispatch.
    
9. What is the `override` keyword (C++11), and why use it?  
    A specifier placed after a derived function's declaration to explicitly indicate it's meant to override a base class virtual function. If the signature doesn't actually match any base virtual function (e.g., due to a typo), the compiler raises an error instead of silently creating an unrelated overload.
    
10. What is the `final` keyword and its uses with virtual functions/classes?  
    `final` on a virtual function prevents further derived classes from overriding it. `final` on a class prevents any class from inheriting from it at all. Both are compile-time enforced design constraints.
    
11. Can static functions be virtual?  
    No. Virtual dispatch relies on an object instance and its vptr, but static functions aren't associated with any particular object — they have no `this` and thus can't participate in dynamic dispatch.
    
12. Can constructors be virtual? Can destructors?  
    Constructors cannot be virtual (the object/vtable doesn't fully exist yet during construction). Destructors can and often should be virtual, specifically to ensure correct cleanup when deleting derived objects through base class pointers.
    
13. What is a covariant return type in the context of overriding?  
    A rule allowing an overriding function in a derived class to return a more-derived pointer/reference type than the base class function it overrides (e.g., base returns `Animal*`, override returns `Dog*`), as long as `Dog` is derived from `Animal`.
    

---

## 14. Vtable & Vptr (Internals)

1. What is a vtable?  
    A "virtual table" — a static array of function pointers, one per class that has virtual functions, holding the addresses of the actual (most-derived) implementations to call for each virtual function of that class.
    
2. What is a vptr, and where is it stored?  
    A "virtual pointer" — a hidden pointer added by the compiler to every object of a class (or derived class) that has at least one virtual function. It's stored as part of the object's own memory layout (typically at the start) and points to that class's vtable.
    
3. How does the compiler resolve a virtual function call at runtime using the vtable/vptr?  
    At the call site, the compiler generates code to dereference the object's vptr to find its vtable, then look up the correct function pointer at the corresponding index in that table, and call through that pointer — this indirection is what enables dynamic dispatch.
    
4. Does every class have a vtable?  
    No. Only classes that declare or inherit at least one virtual function get a vtable (and their objects get a vptr). Classes with no virtual functions have no such overhead.
    
5. What is the memory overhead of having virtual functions in a class?  
    Each object gets an extra vptr (typically the size of a pointer, e.g. 8 bytes on 64-bit systems), and the class itself gets one shared vtable stored once in the program's data segment — not per object.
    
6. How does multiple inheritance affect the vtable structure?  
    An object can end up with multiple vptrs — one per base class subobject that has virtual functions — since each base's virtual function table needs its own independent lookup path, complicating the object's memory layout.
    
7. Why can't a constructor be virtual, in terms of vtable setup?  
    The vptr is set up by the constructor itself, progressively, as each level of the class hierarchy is constructed (base first). Since the object's dynamic type isn't fully established until construction completes, there's no valid vtable to dispatch through while the constructor is still running.
    
8. How does a virtual destructor work with the vtable when deleting via a base pointer?  
    Because the destructor is virtual, the call goes through the vptr/vtable just like any other virtual call, correctly identifying and invoking the most-derived class's destructor first, which then cascades up through each base destructor in turn.
    

---

## 15. Operator Overloading

1. What is operator overloading? Which operators cannot be overloaded?  
    Redefining the behavior of an existing operator for user-defined types. Operators that cannot be overloaded include `::` (scope resolution), `.` (member access), `.*` (pointer-to-member access), `?:` (ternary), and `sizeof`.
    
2. What is the difference between overloading an operator as a member function vs a friend function?  
    As a member function, the left-hand operand is implicitly `this`, so it only works when the left operand is an object of that class. As a friend (non-member) function, both operands are passed explicitly as parameters, allowing the left operand to be of a different/built-in type (e.g., enabling `5 + obj`).
    
3. Why is `operator=` usually overloaded as a member function and not a friend?  
    Assignment inherently modifies the left-hand object and only makes semantic sense when the left operand is an existing object of the class, matching how member functions naturally operate on `this` — plus the compiler-generated default copy assignment is always a member function.
    
4. Why must `operator<<` and `operator>>` be overloaded as non-member (typically friend) functions?  
    Because the left operand is a stream object (`std::ostream`/`std::istream`), not an object of your class — if overloaded as a member of your class, you'd be forced to write `obj << cout`, which is backwards. As a free function, `cout << obj` works naturally.
    
5. What is the difference between overloading prefix and postfix increment operators?  
    Prefix `++obj` is overloaded as `operator++()` with no parameters, returning a reference to the incremented object. Postfix `obj++` is overloaded as `operator++(int)`, using a dummy `int` parameter purely to distinguish it, and conventionally returns a copy of the object's value before incrementing.
    
6. Can you overload operators for built-in types like `int + int`?  
    No. At least one operand in an overloaded operator must be a user-defined type (class/struct/enum) — you cannot change the behavior of operators when all operands are built-in types.
    

---

## 16. Object Slicing

1. What is object slicing?  
    When a derived class object is assigned/copied into a base class object (by value, not by pointer/reference), the derived-specific members are "sliced off," leaving only the base class portion — the object effectively loses its derived identity.
    
2. When does object slicing occur? Give an example scenario.  
    It occurs whenever a derived object is passed, returned, or assigned by value as a base type — e.g., `Base b = derivedObj;` or passing a `Derived` object to a function taking `Base` by value. The `Derived`-specific data and vtable pointer setup are lost; `b` behaves purely as a `Base` object.
    
3. How can object slicing be prevented?  
    By always working with base class pointers or references (`Base&`, `Base*`) instead of passing/storing objects by value when polymorphic behavior is intended, and by making base classes non-copyable or abstract where slicing shouldn't be allowed at all.
    
4. Does object slicing occur with pointers or references too?  
    No — a `Base*` or `Base&` bound to a `Derived` object still points/refers to the full derived object in memory; only the static type used for access is `Base`. Virtual function calls through that pointer/reference still correctly dispatch to the derived implementation. Slicing is strictly a by-value phenomenon.
    

---

## 17. Composition, Aggregation & Association

1. What is composition? Give a "has-a" example.  
    A strong "has-a" relationship where the contained object's lifetime is tightly bound to the owner — it's created and destroyed along with the owner. Example: a `House` class contains `Room` objects as members; if the `House` is destroyed, its `Room`s cease to exist too.
    
2. What is aggregation, and how is it different from composition?  
    Aggregation is also a "has-a" relationship, but a weaker one — the contained object can exist independently of the owner and may outlive it. Example: a `Department` has `Professor`s, but a `Professor` can exist (and be reassigned) even if the `Department` is dissolved. Composition implies ownership/lifetime coupling; aggregation doesn't.
    
3. What is association?  
    The most general relationship between two otherwise independent classes, where objects interact/reference each other without any ownership implication — e.g., a `Teacher` and a `Student` are associated, but neither owns or controls the other's lifetime.
    
4. When should you prefer composition over inheritance?  
    When you want to reuse behavior without establishing a rigid "is-a" hierarchy or exposing the full base interface — composition offers more flexibility (can swap the contained object at runtime), avoids fragile base class issues, and reduces tight coupling.
    
5. What is the key lifetime-dependency difference between composition and aggregation?  
    In composition, the part's lifetime is entirely managed by and tied to the whole (destroying the whole destroys the part). In aggregation, the part's lifetime is independent — the whole merely references/uses it without owning its lifecycle.
    
6. Give an LLD example where composition is a better design choice than inheritance.  
    Modeling a `Car` that "has an" `Engine`: making `Car` inherit from `Engine` would be semantically wrong (a car is not a type of engine) and would rigidly lock in one engine type. Composition (`Car` holds an `Engine` object/interface) lets you swap in a `PetrolEngine`, `ElectricEngine`, etc., at runtime — matching real-world "has-a" semantics and enabling the Strategy pattern.
    

---

## 18. RTTI & Type Casting

1. What is RTTI (Run-Time Type Information)?  
    A C++ mechanism that allows the type of an object to be determined during program execution, primarily through `typeid` and `dynamic_cast`, and it requires the class to be polymorphic (have at least one virtual function).
    
2. What is `dynamic_cast`, and when is it used?  
    A cast used to safely convert pointers/references between related class types in a polymorphic hierarchy (typically base-to-derived, "downcasting"), performing a runtime check to ensure the conversion is actually valid.
    
3. What is the difference between `static_cast`, `dynamic_cast`, `const_cast`, and `reinterpret_cast`?  
    `static_cast` performs compile-time-checked conversions between related types (no runtime safety check). `dynamic_cast` performs a runtime-checked cast for polymorphic class hierarchies. `const_cast` adds or removes `const`/`volatile` qualifiers. `reinterpret_cast` performs low-level, unsafe bit-pattern reinterpretation between unrelated types.
    
4. What happens if `dynamic_cast` fails for pointers vs references?  
    For pointer types, a failed `dynamic_cast` returns `nullptr`. For reference types, since there's no "null reference," it throws a `std::bad_cast` exception instead.
    
5. What is `typeid` used for?  
    It returns a `std::type_info` object representing the actual runtime type of an expression/object, commonly used to compare or print the dynamic type of a polymorphic object.
    
6. Does `dynamic_cast` require virtual functions in the base class? Why?  
    Yes — the class must be polymorphic (have at least one virtual function). This is because `dynamic_cast`'s runtime type check relies on RTTI data accessible through the object's vtable; without any virtual function, there's no vtable and hence no way to check the type at runtime.
    

---

## 19. SOLID Principles & LLD Basics

1. What does SOLID stand for?  
    Single Responsibility Principle, Open/Closed Principle, Liskov Substitution Principle, Interface Segregation Principle, and Dependency Inversion Principle.
    
2. Explain the Single Responsibility Principle with an example.  
    A class should have only one reason to change — i.e., one responsibility. Example: an `Invoice` class that both calculates totals and handles printing/saving to a file violates SRP; splitting printing/persistence into separate classes (`InvoicePrinter`, `InvoiceRepository`) follows it.
    
3. Explain the Open/Closed Principle.  
    Classes should be open for extension but closed for modification — you should be able to add new behavior (e.g., a new shape type) without editing existing, tested code, typically achieved via abstraction and polymorphism (adding a new derived class rather than adding an if/else branch in existing code).
    
4. Explain the Liskov Substitution Principle — how does it relate to inheritance correctness?  
    Objects of a derived class should be substitutable for objects of the base class without breaking correctness — a subclass must honor the base class's contract/behavior expectations. A classic violation: a `Square` inheriting from `Rectangle` that overrides `setWidth`/`setHeight` in a way that breaks the expected independent-dimension behavior of a `Rectangle`.
    
5. Explain the Interface Segregation Principle.  
    Clients shouldn't be forced to depend on interfaces/methods they don't use. Prefer several small, specific interfaces over one large, general-purpose interface — e.g., splitting a bloated `Machine` interface with `print()`, `scan()`, `fax()` into separate `Printer`, `Scanner`, `Fax` interfaces.
    
6. Explain the Dependency Inversion Principle.  
    High-level modules shouldn't depend directly on low-level modules — both should depend on abstractions (interfaces). This decouples components, e.g., a `NotificationService` should depend on an abstract `MessageSender` interface rather than directly on a concrete `EmailSender` class, so it can work with any implementation.
    
7. How does polymorphism help implement the Open/Closed Principle?  
    By programming against a base class/interface and using virtual dispatch, new behavior can be added simply by introducing new derived classes that implement the interface — existing client code that operates on the base type needs no modification to support the new type.
    
8. Why is "favor composition over inheritance" considered a good design guideline?  
    Inheritance creates tight, compile-time coupling and can lead to fragile hierarchies where changes in a base class ripple unpredictably into subclasses. Composition is more flexible (behavior can be swapped at runtime via interfaces), promotes loose coupling, and avoids issues like the diamond problem and LSP violations.
    

---

## 20. Miscellaneous / Nuances

1. What is a nested class in C++?  
    A class defined entirely within the scope of another class. It's a member of the enclosing class in terms of naming/access scope, but its objects are independent and don't automatically have a relationship to any specific enclosing class instance.
    
2. What is name hiding (function hiding) in inheritance?  
    If a derived class defines a function with the same name as a base class function but a different signature (not an override), it "hides" all overloads of that name from the base class in the derived class's scope — even ones with different parameters — unless brought back into scope with a `using Base::funcName;` declaration.
    
3. How does Java resolve the "diamond problem" for interfaces differently from C++'s virtual inheritance?  
    Since Java interfaces (pre-default-methods) carried no state and only method signatures, implementing multiple interfaces with the same method name caused no ambiguity — the implementing class just provides one implementation. With default methods (Java 8+), a similar conflict can occur, resolved by requiring the implementing class to explicitly override and resolve the conflicting default method.
    
4. What is the difference between struct and class besides the default access specifier?  
    Functionally, that's essentially the only technical difference in C++ (default member/inheritance access). Everything else — constructors, virtual functions, templates, inheritance — works identically for both; the distinction is largely stylistic convention (struct for POD/data, class for encapsulated behavior).
    
5. Can a class inherit from itself?  
    No — this is a compile error. A class must be fully defined before it can be used as a base class, so a class cannot appear as its own base in its own definition (self-referential inheritance is disallowed).
    
6. What is an "interface" in C++, given there's no explicit `interface` keyword?  
    C++ simulates interfaces using a pure abstract class — a class containing only pure virtual functions and no data members or implementation, meant purely to define a contract that concrete derived classes must fulfill.
    
7. What is the difference between an abstract class and an interface (conceptually)?  
    An abstract class can mix pure virtual functions with concrete implementations and data members (partial implementation). An interface (pure abstract class in C++) provides no implementation or state at all — purely a contract of method signatures for implementers to fulfill.
    
8. What is a singleton class, and how do you implement it in C++?  
    A class designed to allow only one instance to exist throughout the program. Implemented with a private constructor (preventing external instantiation), a private static pointer/reference to the sole instance, and a public static method (e.g., `getInstance()`) that creates the instance on first call and returns it on subsequent calls.
    
9. What is the significance of `= default` and `= delete` specifiers (C++11)?  
    `= default` explicitly asks the compiler to generate its default implementation of a special member function (useful when you've declared other constructors, which suppresses auto-generation). `= delete` explicitly forbids a function from being used/called at all (commonly used to make a class non-copyable by deleting its copy constructor/assignment operator).
    
10. Why can't calling a pure virtual function from a base class constructor invoke the derived class's override?  
    During base class construction, the derived part of the object hasn't been constructed yet, and the vptr still points to the base class's vtable at that stage — so the call resolves to the base class version (or is undefined behavior/link error if truly no implementation exists), never the derived override.
    
11. What design practice helps avoid object slicing in polymorphic LLD designs?  
    Consistently pass and store polymorphic objects via pointers or references (raw or smart pointers like `std::unique_ptr<Base>`) rather than by value, ensuring virtual dispatch and full object identity are preserved throughout the design.
    
12. What is the memory cost difference for diamond inheritance with vs without virtual inheritance?  
    Without virtual inheritance, the most-derived class contains two full separate copies of the shared grandparent's data members (duplicated storage and ambiguous access). With virtual inheritance, only a single shared copy of the grandparent's data exists, though this typically adds a small extra layer of indirection (virtual base table pointers) to locate that shared subobject.
    

---