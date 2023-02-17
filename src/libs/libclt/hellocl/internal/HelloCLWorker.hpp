#ifdef OC_USE_LIB_EXT_OPENCL

#ifndef HELLOCLWORKER_HPP
#define HELLOCLWORKER_HPP

#include "clt/CLWorker.hpp"


class CLThreadManager;

/**
 * @brief The HelloCLWorker class is a basic example implementation of CLWorker.
  *
 * It works together with HelloGLCLViewRenderer and HelloCLWorkerFactory
 *
 */
class HelloCLWorker : public CLWorker
{
private:

public:
	HelloCLWorker(CLThreadManager &man, int index, QObject *parent = nullptr);

	// CLWorker interface
	//////////////////////////////////////////
public slots:
	void preProcessImp() Q_DECL_OVERRIDE;
	void processImp() Q_DECL_OVERRIDE;
	void postProcessImp() Q_DECL_OVERRIDE;
};

#endif // HELLOCLWORKER_HPP

#endif
// OC_USE_LIB_EXT_OPENCL
