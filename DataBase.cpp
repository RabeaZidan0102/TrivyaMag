#include "DataBase.h"
#include "sqlite3.h"
#include <exception>

using std::exception;

DataBase::DataBase()
{
	int rc;
	char* zErrMsg = 0;
	bool ans;
	rc = sqlite3_open("carsDealer.db", &_db);

	try
	{
		if (rc != SQLITE_OK)
		{
			this->_db = nullptr;
			throw (exception("Can't open database"));
		}

		if()
	}
}
