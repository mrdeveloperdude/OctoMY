#ifndef PLANBOOKSERVICE_HPP
#define PLANBOOKSERVICE_HPP


#include "plan/book/PlanBook.hpp"
#include "service/ServiceWrapper.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"

class Node;


/**
 * @brief The PlanBookService class is what it says on the tin; basically a wrapper to make PlanBook appear as a service.
 */

class PlanBookService: public ServiceWrapper<PlanBook>
{
private:
	QSharedPointer<Node> mNode;
	ConfigureHelper mConfigureHelper;
	
public:
	PlanBookService(QSharedPointer<PlanBook> planBook, QStringList dependencies= {});
	
public:
	void configure(QSharedPointer<Node> node);
	
	// ServiceWapper interface.
public:
	void serviceWrapperActivate(QSharedPointer<PlanBook> wrapee, bool on, ServiceActivatedCallback callBack) override;
	
};




#endif // PLANBOOKSERVICE_HPP
