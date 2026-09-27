---
name: class-documentation
description: 'Create Doxygen-compatible C++ class documentation following the InteractiveToolkit pattern. Use when documenting new classes, adding docs to existing classes, or reviewing class documentation quality. Triggers: "document", "Doxygen", "class docs", "Sphere.h pattern", "add documentation".'
argument-hint: 'Class name or file path to document'
user-invocable: true
disable-model-invocation: false
---

# C++ Class Documentation (Doxygen)

## When to Use
- Documenting new C++ classes or adding Doxygen comments to existing ones
- Reviewing class documentation for consistency with the InteractiveToolkit pattern

## Critical Rule: Preserve Implementations

**NEVER remove or strip the original implementation when adding documentation.** Only add `///` comment blocks above existing code; the function body, constructor initializer list, etc. must remain intact. Adding docs must never change compiled behavior.

## Author Question

When creating new documentation, always ask the user: **"What is the author name?"**

Use the provided author name to fill all `\author` tags in the generated documentation. If the user does not provide an author, use a placeholder like `Unknown` and prompt them to update it.

## The Pattern

Tag order is always: `\brief` → extended description → `\author` → `\param` → `\return`.

```cpp
/// \brief Short one-line description of the class.
///
/// Extended description explaining purpose, design, and usage.
///
/// Example:
///
/// \code
/// ClassName<float> obj;
/// \endcode
///
/// \author Author Name
///
/// \tparam T Numeric type for coordinates (e.g., float, double).
///
template <typename T>
class ClassName {
public:
    /// \brief Alias for the fully specialized class type.
    ///
    using self_type = ClassName<T>;

    /// \brief The radius of the sphere.
    ///
    T radius;

    /// \brief Construct a ClassName with the given radius.
    ///
    /// Example:
    ///
    /// \code
    /// ClassName obj(1.0f);
    /// \endcode
    ///
    /// \author Author Name
    /// \param radius The radius. Must be non-negative.
    ///
    ITK_INLINE ClassName(T radius) { /* existing body */ }

    /// \brief Component-wise sum (add) operator overload.
    ///
    /// Increment the instance by the components of another instance.
    ///
    /// Example:
    ///
    /// \code
    /// ClassName a, b;
    /// a += b;
    /// \endcode
    ///
    /// \author Author Name
    /// \param v Instance to add to the current instance.
    /// \return A reference to the current instance after the increment.
    ///
    ITK_INLINE self_type &operator+=(const self_type &v) { /* existing body */ return (*this); }

    /// \brief Compute the squared distance from a point to the sphere surface.
    ///
    /// Returns zero if the point is inside or on the sphere.
    ///
    /// Example:
    ///
    /// \code
    /// ClassName obj;
    /// T result = obj.squaredDistanceTo(point);
    /// \endcode
    ///
    /// \author Author Name
    /// \param point The query point.
    /// \return Squared distance to the sphere surface.
    ///
    ITK_INLINE T squaredDistanceTo(const Vector3<T> &point) const { /* existing body */ }
};
```

All constructs follow this shape:
- **Type aliases, constants, members**: `\brief` + extended description.
- **Constructors** (default, SIMD register, scalar, SFINAE-converting, two-argument, difference `b - a`, copy): `\brief` + extended + `\code` example + `\author` + `\param` per argument.
- **Operators** (assignment, comparison, compound, unary, conversion): same, plus `\return` if applicable.
- **Methods** (static and instance): same, plus `\return`.
- **Unions**: document the union (describing its access views) and each member, including nested struct members.
- **SFINAE partial specializations** (`std::enable_if` on `SIMD_TYPE::SSE`/`NEON`): class-level docs must additionally explain *when the specialization is selected* and describe each template parameter's constraint via `\tparam`.

## Required Doxygen Tags

| Tag | Purpose | Required? |
|-----|---------|-----------|
| `\brief` | One-line summary | Yes (class, method, member, union, struct) |
| `\author` | Author attribution | Yes (class, constructor, method, operator) |
| `\param` | Parameter description | Yes (methods with params) |
| `\return` | Return value description | Yes (methods with return) |
| `\code` / `\endcode` | Code example | Recommended |
| `\note` | Additional notes | Optional |
| `\see` | Cross-references | Optional |
| `\tparam` | Template parameter description | Yes (template classes/structs) |

## Conditional Compilation

When documenting code with `#if defined(ITK_SSE2)` / `#elif defined(ITK_NEON)` / `#else` / `#endif`, document the **entire block** as a single logical element. The `\brief` and `\author` tags go **before** the `#if` directive; the `#else` / `#endif` remain undocumented. Do not split branches into separate entries.

## What NOT to Document

- **Commented-out code** (e.g. `// float32x2_t diff = ...`) — only document active, compiled code.
- **Section comments** (e.g. `// inter SIMD types converting...`) — internal organization markers, not Doxygen entries.
- **`#error` preprocessor directives** — not Doxygen entries.

## Quality Checklist

- [ ] Class has `\brief`, `\author`, and `\tparam` (if templated)
- [ ] Every public method/operator/constructor has `\brief` and `\author`
- [ ] Methods have `\param` (per argument) and `\return` (if applicable)
- [ ] Code examples use `\code` / `\endcode` blocks showing typical usage
- [ ] SFINAE specializations explain the selection criteria in class-level docs
- [ ] All overloaded signatures are documented (not just one)
- [ ] Union members and nested structs are documented
- [ ] Conditional compilation blocks are documented as a single logical unit
- [ ] Commented-out code, section comments, and `#error` directives are not documented
- [ ] All original implementations are preserved (only `///` blocks added)
- [ ] Language is English; descriptions are concise but complete
- [ ] No cross-references in place of documentation: never write "Same as \c methodName" — each method needs its own complete `\brief`, description, and example


