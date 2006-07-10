#include <aterm2.h>
#include <iostream>
#include <string>

enum TraceFormat { tfMcrl2, tfPlain, tfUnknown };

class Trace
{
	public:
		Trace();
		Trace(std::istream &is, TraceFormat tf = tfUnknown);
		Trace(std::string &filename, TraceFormat tf = tfUnknown);
		~Trace();

		void resetPosition();
		void setPosition(unsigned int pos);
		unsigned int getPosition();
		unsigned int getLength();

		ATermAppl currentState();
		ATermAppl currentTime();
		ATermAppl nextAction();

		void truncate();

		void addAction(ATermAppl action, ATermAppl time = NULL);
		bool setState(ATermAppl state);
		bool canSetState();

		ATermAppl getAction();
		ATermAppl getState();
		ATermAppl getTime();

		void load(std::istream &is, TraceFormat tf = tfUnknown);
		bool load(std::string &filename, TraceFormat tf = tfUnknown);
		void save(std::ostream &os, TraceFormat tf = tfMcrl2);
		bool save(std::string &filename, TraceFormat tf = tfMcrl2);

	private:
		ATermAppl *states;
		ATermAppl *actions;
		ATermAppl *times;
		unsigned int buf_size;
		unsigned int len;
		unsigned int pos;

		void init();

		TraceFormat detectFormat(std::istream &is);
		void loadMcrl2(std::istream &is);
		void loadPlain(std::istream &is);
		void saveMcrl2(std::ostream &os);
		void savePlain(std::ostream &os);
};
