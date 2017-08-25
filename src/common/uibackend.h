
#ifndef UIBACKEND_H
#define UIBACKEND_H


#include <QtCore/QString>
#include <QtCore/Qt> // SortOrder

#include <cassert> // assert
#include <functional>  // function
#include <memory> // unique_ptr, make_unique
#include <type_traits> // is_base_of
#include <utility> // forward



class CanRawView;
class QAbstractItemModel;
class QWidget;

/**
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */


/** UIBackend for given Subject tag (can be any type). */
template<class Subject>
struct UIBackend;

/** Default UIBackend for given Subject tag. */
template<class Subject>
struct UIBackendDefault : UIBackend<Subject>;



template<class Subject>
class WithUIBackend
{

 public:

	/**
	 * Creates an object with implementation of @c UIBackend<Subject>
     * of type @c Impl is specified, otherwise default (must exist)
     * implementation of type @c UIBackendDefault<Subject> is created.
     * Lifetime of the object is managed by object of this class.
     * Arguments passed to this constructor are forwarded to the Impl
     * constructor.
     */
	template<class Impl = UIBackendDefault<Subject>, class... As>
	explicit WithUIBackend(As&&... args)
	  :
		uiRep{std::make_unique<Impl>(std::forward<As>(args)...)}
	  , uiHandle{uiRep.get()}
	{
		static_assert(std::is_base_of<UIBackend<Subject>, Impl>::value
				    , "Impl does not match interface");
	}

	/** DOES NOT manage lifetime of @c backend variable! */
	explicit WithUIBackend(UIBackend<Subject>& backend)
	  : uiHandle{&backend}
	{}

    UIBackend<Subject>& backend()
	{
		assert(nullptr != uiHandle);

		return *uiHandle;
	}

    const UIBackend<Subject>& backend() const
	{
		assert(nullptr != uiHandle);

		return *uiHandle;
	}

 protected:

    /** @{ If backend is passed explicitly in constructor, uiHandle stores
     *     pointer to it, and backend memory is not managed by uiRep.
     *     Otherwise, uiRep manages memory.
     *
     *     NEVER perform delete on uiHandle.
     *     DO NOT reorder uiRep and uiHandle.
     *
     * */
    std::unique_ptr<UIBackend<Subject>> uiRep;    /**< NOTE: stores derived from UIBackend<Subject> */
    UIBackend<Subject>* 		        uiHandle; /**< uiRep observer. */
    /** @} */
};






/** ----------------------- BACKEND INTERFACES FOLLOW ---------------------- */


template<>
struct UIBackend<CanRawView>
{
    virtual QString getClickedColumn(int ndx) = 0;
    virtual QWidget* getMainWidget() = 0;
    virtual int getSortOrder() = 0;
    virtual void initTableView(QAbstractItemModel& tvModel) = 0;
    virtual void setClearCbk(std::function<void ()> cb) = 0;
    virtual void setDockUndockCbk(std::function<void ()> cb) = 0;
    virtual void setSectionClikedCbk(std::function<void (int)> cb) = 0;
    virtual void setSorting(int sortNdx, int clickedNdx, Qt::SortOrder order) = 0;
    virtual void updateScroll() = 0;

    virtual ~UIBackend() = default;
};


#endif

