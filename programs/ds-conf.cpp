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
#include"OperationCore.h"
#include"Messages.h"

int main(int argc, char* argv[])
{
  messagesProgramName = "ds-conf";
  setlocale(LC_ALL, "");
  initLogging(LOG_DEBUG, 0);
  try{
    ConfigCenter conf;
    conf.loadFromFile(DEFAULT_CONFIG_FILE_NAME);
    conf.loadFromDir(DEFAULT_CONFIG_DIR_NAME);
    conf.commit();
    conf.printConfigData(std::cout);
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
  catch(const NotImplementedException& e)
    {
      std::cerr << "Feature not implemented:" << e.getMessage() << std::endl;
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
