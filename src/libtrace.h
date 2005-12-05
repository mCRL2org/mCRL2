#include <aterm2.h>
#include <iostream>
#include <string>

enum TraceFormat { tfVer1, tfPlain, tfUnknown };

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
		ATermAppl nextAction();

		void truncate();

		void addAction(ATermAppl action);
		bool setState(ATermAppl state);
		bool canSetState();

		ATermAppl getAction();
		ATermAppl getState();

		void load(std::istream &is, TraceFormat tf = tfUnknown);
		void load(std::string &filename, TraceFormat tf = tfUnknown);
		void save(std::ostream &os, TraceFormat tf = tfVer1);
		void save(std::string &filename, TraceFormat tf = tfVer1);

	private:
		ATermAppl *states;
		ATermAppl *actions;
		unsigned int buf_size;
		unsigned int len;
		unsigned int pos;

		void init();

		TraceFormat detectFormat(std::istream &is);
		void loadVer1(std::istream &is);
		void loadPlain(std::istream &is);
		void saveVer1(std::ostream &os);
		void savePlain(std::ostream &os);
};
