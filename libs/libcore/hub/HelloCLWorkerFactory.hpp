#ifndef HELLOCLWORKERFACTORY_HPP
#define HELLOCLWORKERFACTORY_HPP

#include "../libcl/opencl/CLWorkerFactory.hpp"

#include <QtGlobal>
/*!
 * \brief The HelloCLWorkerFactory class creates instances of CLWorker on demand
 * from CLThreadManager.
 */
class HelloCLWorkerFactory : public CLWorkerFactory
{
	public:
		HelloCLWorkerFactory();

	public:
		virtual CLWorker * createInstance() Q_DECL_OVERRIDE;

};

#endif // HELLOCLWORKERFACTORY_HPP
