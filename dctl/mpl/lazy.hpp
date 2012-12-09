#pragma once

#define DCTL_PP_DECLARE_LAZY_UNARY_METAFUNCTION(F)      \
namespace lazy {                                        \
template<typename X>                                    \
struct F;                                               \
}       // namespace lazy

#define DCTL_PP_DECLARE_LAZY_BINARY_METAFUNCTION(F)     \
namespace lazy {                                        \
template<typename X1, typename X2>                      \
struct F;                                               \
}       // namespace lazy

#define DCTL_PP_DEFINE_LAZY_UNARY_METAFUNCTION(F)       \
namespace lazy {                                        \
template<typename X>                                    \
struct F                                                \
:                                                       \
        F<typename X::type>                             \
{};                                                     \
}       // namespace lazy

#define DCTL_PP_DEFINE_LAZY_BINARY_METAFUNCTION(F)      \
namespace lazy {                                        \
template<typename X1, typename X2>                      \
struct F                                                \
:                                                       \
        F<typename X1::type, typename X2::type>         \
{};                                                     \
}       // namespace lazy
