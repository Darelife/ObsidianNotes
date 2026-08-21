# Object Oriented Programming (OOP)
1. Class
2. Object: Instance of a class
3. Constructor: Method that runs when an object is created
4. Enum
```cpp
enum class OrderStatus {
    PLACED,
    CONFIRMED,
    SHIPPED,
    DELIVERED,
    CANCELLED
};
```
Here's the tightened version:

**5. Access Modifiers:**

- `public` — accessible from anywhere
- `protected` — accessible within the class and its derived classes
- `private` — accessible only within the class itself

**6. Interface:** A contract that specifies **what** a class must do, without dictating **how**. Used when unrelated/different classes need to share the same contract (behavioural agreement, no shared state or code).

```cpp
class CLASS {
public:
    virtual void FUNC(std::string PARAMETER) = 0;
    virtual ~CLASS() = default;
};
```

- All methods are **pure virtual** (`= 0`) — no implementation, no data members.
- A class implementing this contract _must_ override every pure virtual method, or it remains abstract itself.

**7. Abstract Classes:** Used when related classes need to share **common code or state**, while leaving some behaviour to be defined by each subclass.

```cpp
class CLASS1 {
public:
    void METHOD1(TYPE1 PARAM1) {          // concrete, shared logic
        TYPE2 VAR1 = METHOD3(PARAM1);
        METHOD2(VAR1);
    }
    void METHOD2(TYPE2 PARAM2) {          // concrete, shared logic
        // ACTION1
    }
    virtual TYPE2 METHOD3(TYPE1 PARAM1) = 0;  // pure virtual — must be defined by each subclass
    virtual ~CLASS1() = default;
};

class CLASS2 : public CLASS1 {
public:
    TYPE2 METHOD3(TYPE1 PARAM1) override {
        TYPE2 VAR2;
        for (TYPE3 VAR3 = 0; VAR3 < PARAM1.size(); VAR3++) {
            if (CONDITION1) {
                // ACTION2
            }
            // ACTION3
        }
        return VAR2;
    }
};

class CLASS3 : public CLASS1 {
public:
    TYPE2 METHOD3(TYPE1 PARAM1) override {
        TYPE2 VAR2 = VALUE1;
        for (TYPE3 VAR3 = 0; VAR3 < PARAM1.size(); VAR3++) {
            if (CONDITION1) {
                // ACTION2
            }
            // ACTION3
        }
        return VAR2 + VALUE2;
    }
};
```

- `METHOD1` and `METHOD2` are **shared, concrete code** — written once in the base class, reused by every subclass.
- `METHOD3` is left **pure virtual** — each subclass supplies its own version. This is a common pattern called the **Template Method pattern** (base class defines the skeleton/algorithm, subclasses fill in specific steps).
- Unlike an interface, an abstract class **can** hold data members and constructors, and **can** mix pure virtual methods with fully implemented ones.

**In C++, interfaces and abstract classes are implemented the same way** — both use classes with virtual functions (C++ has no separate `interface` keyword like Java/C#). The distinction is purely conceptual/by convention:

|              | Interface                                    | Abstract class                           |
| ------------ | -------------------------------------------- | ---------------------------------------- |
| Methods      | all pure virtual                             | mix of pure virtual + concrete           |
| Data members | none                                         | can have them                            |
| Purpose      | contract/capability across unrelated classes | shared code/state across related classes |
| C++ syntax   | class, all `= 0`                             | class, some `= 0`                        |

**8. Encapsulation:** Bundling data & the methods that operate on it inside a class, and restricting access via access modifiers (`private`/`protected`/`public`). Controls **who** can access/modify something.

**9. Abstraction:** Hiding internal implementation complexity, exposing only simple, necessary methods publicly. Controls **which details** the outside world needs to see. _(Encapsulation = the mechanism; Abstraction = the goal.)_

**10. Inheritance:** Allows a class to reuse/extend the fields and behavior of another class. ("is-a")

```cpp
class CLASS1 {
protected:
    TYPE1 MEMBER1;

public:
    CLASS1(TYPE1 PARAM1) {
        this->MEMBER1 = PARAM1;
    }

    virtual void METHOD1(TYPE1 PARAM2) {
        // ACTION1
    }

    void METHOD2(TYPE1 PARAM2) {
        // ACTION2
    }

    virtual ~CLASS1() = default;  // virtual destructor needed for safe polymorphic deletion
};

class CLASS2 : public CLASS1 {
public:
    CLASS2(TYPE1 PARAM1) : CLASS1(PARAM1) {}

    void METHOD1(TYPE1 PARAM2) override {
        // ACTION3
    }
};

class CLASS3 : public CLASS1 {
public:
    CLASS3(TYPE1 PARAM1) : CLASS1(PARAM1) {}

    void METHOD1(TYPE1 PARAM2) override {
        // ACTION4
    }
};
```

- **Inheritance:** one class _is a_ specialized version of another.
- **Composition:** instead of inheriting, a class _contains_ another class as a member and uses its behaviour ("has-a" / "uses-a"). No inheritance relationship needed.
- Rule of thumb: **favor composition over inheritance** when there's no true "is-a" relationship : more flexible, avoids fragile deep hierarchies.

**11. Polymorphism:** Different objects usable through the same interface/parent type.

1. **Runtime (dynamic) polymorphism:** `ParentClass* a = new ChildClass()` : behaviour resolved at runtime, depends on actual object type. Requires the method to be declared `virtual` in the base class (else C++ falls back to static binding).
2. **Compile-time (static) polymorphism:** Overloading (same method name, different parameters) : also includes operator overloading.

## Class Relationships

**12. Association** :  "uses-a" Two classes are linked, and one holds a reference to the other as an instance variable. Neither owns the other's lifecycle.

- Example: `Teacher` and `Student` :  a teacher has students, students have teachers, but neither creates/destroys the other.
- Multiplicity (1:1, 1:many, many:1, many:many) describes how many instances on each side participate.

**13. Aggregation** :  weak "has-a"

- Example: `Department` has `Professor`s. If the department is dissolved, professors still exist (they can move elsewhere). The parts are shared/independent.
- Represented in UML with a hollow diamond.

**14. Composition** :  strong "has-a"

- Example: `House` has `Room`s. Destroy the house, the rooms are gone too. Parts don't outlive the whole.
- Represented in UML with a filled diamond.

**15. Dependency** :  weakest relationship: "depends-on"

- Example: `OrderProcessor.process(Payment p)` :  `Payment` is just a parameter/local variable, not stored as a field. The dependency exists only for the duration of the method call.
- Represented in UML with a dashed arrow.

One subtlety worth adding to your notes: the **strength ordering** is typically taught as: **Dependency < Association < Aggregation < Composition**

Aggregation and Composition are technically special cases of Association (they're all "has-a" / "uses-a" in the broad sense), just with different ownership/lifecycle strength.

---

1. "is-a", "can-do", "has-a"
2. Difference between Interface and an abstract class

---

# Sources
1. [Algomaster](https://algomaster.io/learn/lld/)
2. 