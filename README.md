# connectionPool

this project shows how to use a THREAD SAFE connection pool(MYSQL, REDIS, etc....);

usage : follow the main.cpp
  1: init the pool
  2: to get the handler with the class dbwrapper.
  3: follow the t_alltype.hpp to define all the field you need. 
  4: then do it.

notes:
	nDBID = 1;  //the id is the only key to get a connection from the pool
	nDBType = 20;  //we have these types. REDIS=10,SQLAPI=20
