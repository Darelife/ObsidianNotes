# Object Oriented Programming (OOP)
1. Class
2. Object: Instance of a class
3. Constructor: Method that runs when an object is created
4. Enum
5. Access Modifiers: public, private, protected
6. Interface: a contract that tells what the class must do, without deciding exactly how it should do it. When different classes need to share the same contract
```cpp
class CLASS {
public:
    virtual void FUNC(std::string PARAMETER) = 0;
    virtual ~CLASS() = default;
};
```

7.  Abstract classes: When related classes need to share common code or states.
```cpp
class CLASS1 {
public:
    void METHOD1(TYPE1 PARAM1) {
        TYPE2 VAR1 = METHOD3(PARAM1);
        METHOD2(VAR1);
    }
    void METHOD2(TYPE2 PARAM2) {
        // ACTION1
    }
    virtual TYPE2 METHOD3(TYPE1 PARAM1) = 0;
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
in cpp, both interfaces, and abstract classes are implemented the same way.

## The four pillars of OOPs

8. Encapsulation: Keeping the data & methods that operate on the data together inside a class, and controlling the access modifiers. Controls who can access/modify something.
9. Abstraction: Hiding internal complexity, and just having simple methods made public, to ease the workflow. Controls which details the the users outside need to see.
10. Inheritance: Allows a class to reuse / extend the fields or behaviour of another class. (**is-a**)
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

    virtual ~CLASS1() = default;
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

Composition: instead of inheritance, the class contains another class, and uses it.

Inheritance: One class is a specialized version of another
Composition: Need to use another class' behaviour

11. Polymorphism: Diff objects can be used through the same interface/parent type.
	1. Run time polymorphism: ParentClass* a = new ChildClass()...behaviour depends on the child class
	2. Compile time polymorphism (static): Overloading

## Class Relationships

12. Association: One class is connected to another. (Class being referenced -> Class referencing): uses-a: one-to-one, one-to-many, many-to-one, many-to-many. Class variable is of a specific object
13. Aggregation: weak "has-a": an object contains other objects. Child is independent of the parent.
14. Composition: strong "has-a": child object is an imp part of parent, and doesn't exist independently from it
15. Dependency: Weaker than association: need a separate class, to perform some work. Method Variable is of a specific object

---

1. "is-a", "can-do", "has-a"
2. Difference between Interface and an abstract class

---

# Sources
1. [Algomaster](https://algomaster.io/learn/lld/)
2. 