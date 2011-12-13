
#ifndef DEPSOLVER_CHANGE_LOG_H
#define DEPSOLVER_CHANGE_LOG_H

class ChangeLogEntry
{
public:
  ChangeLogEntry()
    : time(0) {}

  ChangeLogEntry(time_t tm, const std::string& nm, const std::string& txt)
    : time(tm), name(nm), text(txt) {}

public:
  time_t time;
  std::string name;
  std::string text;
}; //class ChangeLogEntry;

typedef std::vector<ChangeLogEntry> ChangeLogEntryVector;
typedef std::list<ChangeLogEntry> ChangeLogEntryList;
typedef ChangeLogEntryVector ChangeLog;

#endif //DEPSOLVER_CHANGE_LOG_H;
