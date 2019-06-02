#ifndef HELLOCLWORKERFACTORY_HPP
#define HELLOCLWORKERFACTORY_HPP

#include "clt/CLWorkerFactory.hpp"

#include <QtGlobal>
/*!
 * \brief The HelloCLWorkerFactory class is an example implementation of CLWorkerFactory
 * It will create instances of HelloCLWorker on demand from CLThreadManager.
 *
 * It works together with HelloGLCLViewRenderer and HelloCLWorker
 */
class HelloCLWorkerFactory : public CLWorkerFactory
{
public:
	HelloCLWorkerFactory();

public:
	virtual CLWorker * createInstance(CLThreadManager &man, int index) Q_DECL_OVERRIDE;

};

#endif // HELLOCLWORKERFACTORY_HPP
