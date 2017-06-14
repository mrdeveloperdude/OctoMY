#ifndef CRASHHELP_HPP
#define CRASHHELP_HPP


/**
 * @brief The CrashHelp class was invented to aid in cases where the application
 * crashes without giving any clues as to what happened, especially in cases
 * where attaching a debugger is difficult or does not work at all.
 *
 * It will output some logging at the time of death to aid the debugging efforts
 *
 *  NOTE: Will only work when compiled with GCC, as it depends in GCC spesific
 *  extensions/macros etc.
 */

class CrashHelp
{
public:
		explicit CrashHelp();
	explicit CrashHelp(bool enableImplicitly);
	virtual ~CrashHelp();

public:

	void setEnabled(bool enabled);

};


#endif // CRASHHELP_HPP
