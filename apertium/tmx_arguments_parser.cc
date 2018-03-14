/*************************************************************************
*                                                                        *
*  (C) Copyright 2004. Media Research Centre at the                      *
*  Sociology and Communications Department of the                        *
*  Budapest University of Technology and Economics.                      *
*                                                                        *
*  Developed by Daniel Varga.                                            *
*                                                                        *
*  From hunalign; for license see ../AUTHORS and ../COPYING.hunalign     *
*                                                                        *
*************************************************************************/
#include <apertium/tmx_arguments_parser.h>
#include <apertium/string_utils.h>
#include <iostream>
#include <stdlib.h>

// Could be better.
bool alphabetic( char c)
{
  return ((c>='a')&&(c<='z')) || ((c>='A')&&(c<='Z')) || (c=='_');
}

bool Arguments::read( int argc, char **argv )
{
  for ( int i=1; i<argc; ++i )
  {
    std::string p = argv[i];
    if (p.empty() || p[0]!='-')
    {
      std::wcerr << p << ": unable to parse argument\n";
      throw "argument error";
      return false;
    }
    p.erase(0,1);

    if (p.empty())
    {
      std::wcerr << "Empty argument\n";
      throw "argument error";
      return false;
    }

    size_t j;

    for (j = 0 ; j<p.size(); ++j )
    {
      if (! alphabetic(p[j]) )
      {
        if (p[j]=='=')
          p.erase(j,1);
        break;
      }
    }

    ArgName name = p.substr(0,j);
    std::string val = p.substr(j, p.size()-j);
    int num = atoi(val.c_str());

    AnyData anyData(val);
    if ( (num!=0) || (val=="0") )
    {
      anyData.dInt = num;
      anyData.kind = AnyData::Int;
    }
    operator[](name) = anyData;

  }

  return true;
}

bool Arguments::read( int argc, char **argv, std::vector<const char*>& remains )
{
  remains.clear();

  for ( int i=1; i<argc; ++i )
  {
    std::string p = argv[i];
    if (p.empty() || p[0]!='-')
    {
      remains.push_back(argv[i]);
      continue;
    }

    p.erase(0,1);

    if (p.empty())
    {
      std::wcerr << "Empty argument\n";
      throw "argument error";
      return false;
    }

    size_t j;
    for (j = 0; j<p.size(); ++j )
    {
      if (! alphabetic(p[j]) )
      {
        if (p[j]=='=')
          p.erase(j,1);
        break;
      }
    }

    ArgName name = p.substr(0,j);
    std::string val = p.substr(j, p.size()-j);
    int num = atoi(val.c_str());

    AnyData anyData(val);
    if ( (num!=0) || (val=="0") )
    {
      anyData.dInt = num;
      anyData.kind = AnyData::Int;
    }
    operator[](name) = anyData;

  }

  return true;
}

bool Arguments::getNumericParam( const std::string& name, int& num )
{
  const_iterator it=find(name);
  if (it==end())
  {
    // std::wcerr << "Argument -" << name << " missing.\n";
    return false;
  }

  if (it->second.kind != AnyData::Int)
  {
    std::wcerr << "Argument -" << name << ": integer expected.\n";
    throw "argument error";
  }

  num = it->second.dInt;
  erase(name);
  return true;
}

bool Arguments::getSwitchConst( const ArgName& name, bool& sw ) const
{
  const_iterator it=find(name);
  if (it==end())
  {
    sw = false;
    return true;
  }
  else if (! it->second.dString.empty())
  {
    std::wcerr << "Argument -" << name << ": value is not allowed.\n";
    return false;
  }
  else
  {
    sw = true;
    return true;
  }
}

bool Arguments::getSwitch( const ArgName& name, bool& sw )
{
  bool ok = getSwitchConst(name, sw);
  if (ok)
    erase(name);

  return ok;
}

bool Arguments::getSwitchCompact( const ArgName& name )
{
  bool sw(false);
  bool ok = getSwitchConst(name, sw);
  if (ok)
  {
    erase(name);
    return sw;
  }
  else
  {
    std::wcerr << "No value is allowed for argument -" << name << ".\n";
    throw "argument error";
  }
}

void Arguments::checkEmptyArgs() const
{
  if (!empty())
  {
    std::wcerr << "Invalid argument: ";

    for ( Arguments::const_iterator it=begin(); it!=end(); ++it )
    {
      std::wcerr << "-" << it->first;
      if (!it->second.dString.empty())
        std::wcerr << "=" << it->second.dString;
      std::wcerr << " ";
    }
    std::wcerr << std::endl;

    throw "argument error";
  }
}
