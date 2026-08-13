---
name: class-documentation
description: 'Create Doxygen-compatible C++ class documentation following the InteractiveToolkit pattern. Use when documenting new classes, adding docs to existing classes, or reviewing class documentation quality. Triggers: "document", "Doxygen", "class docs", "Sphere.h pattern", "add documentation".'
argument-hint: 'Class name or file path to document'
user-invocable: true
disable-model-invocation: false
---

# C++ Class Documentation (Doxygen)

## When to Use
- Creating documentation for new C++ classes
- Adding Doxygen comments to existing undocumented classes
- Reviewing class documentation for consistency with the InteractiveToolkit pattern
- Generating documentation from code or vice versa

## Critical Rule: Preserve Implementations

**NEVER remove or strip the original method or function implementation when adding documentation.**

When documenting a class, only add `///` comment blocks above the existing code. The implementation body (function body, constructor initializer list, etc.) must remain intact. Adding documentation should never change the compiled behavior of the code.

**Wrong** — removing the implementation:
```cpp
/// \brief Description.
///
inline void foo();
```

**Correct** — preserving the implementation:
```cpp
/// \brief Description.
///
inline void foo() { /* existing body */ }
```

## Author Question

When creating new documentation, always ask the user: **"What is the author name?"**

Use the provided author name to fill all `\author` tags in the generated documentation. If the user does not provide an author, use a placeholder like `Unknown` and prompt them to update it.

## The Pattern (from `Sphere.h`)

Every documented class follows this structure:

### Class-Level Documentation
```cpp
/// \brief Short one-line description of the class.
///
/// Extended description explaining purpose, design, and usage.
/// Can span multiple paragraphs.
///
/// \author Author Name
///
template <typename T>
class ClassName { ... };
```

### Member Variables
```cpp
/// \brief Description of the member variable.
///
type member_name;
```

### Constructors
```cpp
/// \brief Construct a ClassName with default values.
///
/// Extended description of what the constructor does.
///
/// Example:
///
/// \code
/// ClassName obj;
/// \endcode
///
/// \author Author Name
///
inline ClassName();
```

### Static Methods
```cpp
/// \brief Short description of what the method does.
///
/// Extended description with mathematical or algorithmic details.
///
/// Example:
///
/// \code
/// // setup
/// Type a, b;
/// ClassName obj;
///
/// Type result = ClassName::methodName( a, b, obj );
/// \endcode
///
/// \author Author Name
/// \param param_name Description of the parameter.
/// \param another_param Description of another parameter.
/// \return Description of the return value.
///
static inline return_type methodName(const Type &a, const Type &b);
```

### Instance Methods
```cpp
/// \brief Short description of the method.
///
/// Extended description.
///
/// Example:
///
/// \code
/// ClassName obj;
/// Type result = obj.methodName( input );
/// \endcode
///
/// \author Author Name
/// \param param_name Description.
/// \return Description.
///
inline return_type methodName(const param_type &param);
```

## Required Doxygen Tags

| Tag | Purpose | Required? |
|-----|---------|-----------|
| `\brief` | One-line summary | Yes (class, method, member) |
| `\author` | Author attribution | Yes (class, constructor, method) |
| `\param` | Parameter description | Yes (methods with params) |
| `\return` | Return value description | Yes (methods with return) |
| `\code` / `\endcode` | Code example | Recommended |
| `\note` | Additional notes | Optional |
| `\see` | Cross-references | Optional |

## Decision Flow

```
Start: Document a class
  │
  ├─ Ask: "What is the author name?" → store as $author
  │
  ├─ Class-level docs?
  │   └─ Yes → \brief + extended description + \author ($author)
  │
  ├─ Member variables?
  │   └─ Yes → \brief before each member
  │
  ├─ Constructors?
  │   └─ Yes → \brief + \author ($author) + \code example
  │
  ├─ Static methods?
  │   └─ Yes → \brief + \author ($author) + \param + \return + \code example
  │
  └─ Instance methods?
      └─ Yes → \brief + \author ($author) + \param + \return + \code example
```

## Implementation Preservation Checklist

Before finalizing any documentation edit:
- [ ] All original function bodies are preserved
- [ ] No implementation was stripped or replaced with a declaration-only form
- [ ] Only `///` comment blocks were added above existing code

## Quality Checklist

- [ ] Class has `\brief` and `\author`
- [ ] Every public method has `\brief`, `\author`, `\param` (if applicable), `\return` (if applicable)
- [ ] Every constructor has `\brief` and `\author`
- [ ] Code examples use `\code` / `\endcode` blocks
- [ ] Examples show typical usage patterns
- [ ] Template classes document the template parameter purpose
- [ ] Overloaded methods are all documented (not just one signature)
- [ ] Language is English
- [ ] Descriptions are concise but complete

## Minimal Reference Example

```cpp
/// \brief A simple sphere collision primitive.
///
/// Represents a sphere in 3D space defined by a center point and radius.
/// Used for broad-phase collision detection.
///
/// \author John Doe
///
/// \tparam T Numeric type for coordinates (e.g., float, double).
///
template <typename T>
class Sphere {
public:
    /// \brief The center point of the sphere.
    ///
    Vector3<T> center;

    /// \brief The radius of the sphere.
    ///
    T radius;

    /// \brief Construct a Sphere with default values (center at origin, radius 0).
    ///
    /// \author John Doe
    ///
    inline Sphere();

    /// \brief Construct a Sphere with the given center and radius.
    ///
    /// \author John Doe
    /// \param center The center point of the sphere.
    /// \param radius The radius of the sphere. Must be non-negative.
    ///
    inline Sphere(const Vector3<T> &center, T radius);

    /// \brief Compute the squared distance from a point to the sphere surface.
    ///
    /// Returns zero if the point is inside or on the sphere.
    ///
    /// \author John Doe
    /// \param point The query point.
    /// \return Squared distance to the sphere surface.
    ///
    inline T squaredDistanceTo(const Vector3<T> &point) const;
};
```

## Anti-patterns

- **Missing `\author`**: Every class and method should attribute the author.
- **No code examples**: At minimum, show how to call the method.
- **Vague `\brief`**: "Does something" is not useful. Be specific.
- **Inconsistent tag order**: Always `\brief` → extended → `\author` → `\param` → `\return`.
- **Undocumented overloads**: Document all overloaded signatures.
- **Referencing other methods instead of documenting**: Never write "Same as \c methodName" or similar cross-references in place of actual documentation. Each method must have its own complete `\brief`, extended description, and examples. For near-identical methods, document each fully.
