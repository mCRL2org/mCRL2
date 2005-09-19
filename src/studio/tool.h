#ifndef __TOOL_H
#define __TOOL_H

#include <string>
#include <wx/window.h>
#include <wx/thread.h>
#include "object.h"
#include "tooloptions.h"

using namespace std;

class gsToolMonitor
{
	public:
		virtual void ExecutionStarted() = 0;
		virtual void ExecutionEnded() = 0;
		virtual void AvailabilityChanged(bool available) = 0;
		virtual void Modified() = 0;
		virtual void DescriptionChanged() = 0;
};

class gsTool: public gsStorable, public wxThread
{
	public:
		gsTool()
		{
			monitor = NULL;
		}

		virtual void GetOption(gsToolOption &opt) = 0;
		virtual void SetOption(gsToolOption &opt) = 0;
		virtual void Execute(gsObject *in, gsObject *out, wxWindow *parent = NULL)
		{
			if ( !IsRunning() )
			{
				obj_in = in;
				obj_out = out;
				if ( Create() == wxTHREAD_NO_ERROR )
				{
					if ( Run() != wxTHREAD_NO_ERROR ) 
					{
						if ( monitor != NULL )
						{
							monitor->ExecutionStarted();
						}
						Delete();
					}
				} else {
					Delete();
				}
			}
		}
		virtual bool Executing()
		{
			return IsRunning();
		}
		virtual int GetState() = 0;
		virtual wxImageList *GetStateIcons() = 0;
		virtual sType &GetArgumentType() = 0;
		virtual sType &GetResultType() = 0;
		virtual string GetDescription(gsObject *obj) = 0;
		virtual void Configure(wxWindow *parent) = 0;
		virtual void About(wxWindow *parent) = 0;

		virtual void SetMonitor(gsToolMonitor *monitor)
		{
			this->monitor = monitor;
		}

		virtual ExitCode Entry()
		{
			return 0;
		}
		virtual void OnExit()
		{
			if ( monitor != NULL )
			{
				monitor->ExecutionEnded();
			}
		}

	protected:
		virtual void SetAvailable(bool available)
		{
			if ( monitor != NULL )
			{
				monitor->AvailabilityChanged(available);
			}
		}
		virtual void SetModified()
		{
			if ( monitor != NULL )
			{
				monitor->Modified();
			}
		}

	protected:
		gsToolMonitor *monitor;
		gsObject *obj_in;
		gsObject *obj_out;
};

#endif
