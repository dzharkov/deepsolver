/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Michael Pozhidaev

   This file is part of the Deepsolver.

   Deepsolver is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   Deepsolver is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
*/

#include"deepsolver.h"
#include"CliParser.h"
#include"OperationCore.h"
#include"Messages.h"
#include"IndexFetchProgress.h"

class AlwaysTrueContinueRequest: public AbstractOperationContinueRequest
{
public:
  AlwaysTrueContinueRequest() {}
  virtual ~AlwaysTrueContinueRequest() {}

public:
  bool onContinueOperationRequest() const
  {
    return 1;
  }
}; //class AlwaysTrueContinueRequest; 

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "");
  initLogging(LOG_DEBUG, 0);
  try{
    AlwaysTrueContinueRequest alwaysTrueContinueRequest;
    ConfigCenter conf;
    conf.loadFromFile("/tmp/ds.ini");
    conf.commit();
    std::cout << conf.root().dir.pkgData << std::endl;
      exit(EXIT_SUCCESS);
    OperationCore core(conf);
    IndexFetchProgress progress(std::cout);
    core.fetchIndices(progress, alwaysTrueContinueRequest);
  }
  catch (const ConfigFileException& e)
    {
      Messages(std::cerr).onConfigSyntaxError(e);
      return EXIT_FAILURE;
    }
  catch (const ConfigException& e)
    {
      Messages(std::cerr).onConfigError(e);
      return EXIT_FAILURE;
    }
  catch(const SystemException& e)
    {
      Messages(std::cerr).onSystemError(e);
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
