
#ifndef UIBACKENDIFACE_H
#define UIBACKENDIFACE_H

/**
 * All the UI backends per given subject must derive from UIBackend<Subject> interface.
 * User must implement at least one derived UIBackend type per unique Subject, i.e.
 * UIBackendDefault. If it is not desirable to use virtual member functions (as it
 * could be used in the UIBackend<Subject>), then UIBackend<Subject> shall implement
 * all the non-virtual functions *and* UIBackendDefault shall be empty and derive from
 * it publicly and use constructor inheritance. That's due to fact, that the following
 * implementation uses a reference to the object of type UIBackend<Subject> everywhere.
 *
 * NOTE: The actual shape, number of ctors, args taken by ctors is not forced by this
 *       implementation. Instances of UIBackend<> and the derived types can take any
 *       number of arguments that can be forwarded passed through UsesUIBackend and
 *       WithUIBackend to the selected type of UI backend.
 *
 * @see UIBackend<CanRawView> and UIBackendDefault<CanRawView> (aka CanRawViewBackend).
 *
 * @{
 */
/** UIBackend for given Subject tag (can be any type). */
template<class Subject>
class UIBackend;

/** Default UIBackend for given Subject tag, must be derived from UIBackend<Subject>. */
template<class Subject>
class UIBackendDefault;/* : public UIBackend<Subject>
{
    static_assert(false, "UIBackendDefault not implemented");
};*/
/** @} */


/** Tag generator used to select an implementation type to be created. @{ */
template<class T>
struct UIBackendSelectorTag { using type = T; };

#if ! defined(_MSC_VER)  // error C3376: only static data member templates are allowed
template<class Impl>
constexpr UIBackendSelectorTag<Impl> UIBackendSelector{};
#endif
/** @}  */

/** Tags usesd to select given UsesUIBackend constructor. @{ */
template<unsigned> struct UsesUIBackendCtorTag {};

using UsesUIBackendCtorTag_Explicit = UsesUIBackendCtorTag<111>;
using UsesUIBackendCtorTag_ActionQ  = UsesUIBackendCtorTag<222>;
using UsesUIBackendCtorTag_ActionD  = UsesUIBackendCtorTag<333>;
using UsesUIBackendCtorTag_Selector = UsesUIBackendCtorTag<444>;
using UsesUIBackendCtorTag_Actions  = UsesUIBackendCtorTag<556>;
using UsesUIBackendCtorTag_Args     = UsesUIBackendCtorTag<666>;  // fallback: fully variadic

constexpr UsesUIBackendCtorTag_Explicit UsesUIBackendCtor_Explicit{};
constexpr UsesUIBackendCtorTag_ActionQ  UsesUIBackendCtor_ActionQ{};  // runs "in" q_ptr ctor
constexpr UsesUIBackendCtorTag_ActionD  UsesUIBackendCtor_ActionD{};  // runs "in" d_ptr ctor
constexpr UsesUIBackendCtorTag_Selector UsesUIBackendCtor_Selector{};
constexpr UsesUIBackendCtorTag_Actions  UsesUIBackendCtor_Actions{};
constexpr UsesUIBackendCtorTag_Args     UsesUIBackendCtor_Args{};
/** @} */

#endif

