#include "PrintVersion.h"

#include "Log/Log.h"
#include "constants.h"

void PrintVersion(const std::string app)
{
	// Prints version information.
	PRINT("This is %1% version %2%.\n"
		  "Url: %3%\n"
		  "Licensed under the terms of the GPL-3.0.\n"
		  "See https://www.gnu.org/licenses/gpl-3.0.txt for relevant "
		  "information.\n\n",
		  app, APP_VERSION, APP_URL);
}
