#ifndef ABS_COMMAND_H
#define ABS_COMMAND_H

class AbsCommand
{
	public: 

	virtual ~AbsCommand() {}

	virtual bool Execute() = 0;
};
#endif
