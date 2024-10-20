#include "DebugOutputFilter.h"

OutputFilter& operator<<(QDebug out, OutputFilter& filter) 
{ 
   filter.out_message = out; filter.counter++; if(filter.counter > filter.peak) filter.counter = 0; return filter;
}

template<> OutputFilter& operator<<(OutputFilter& filter, const char* str)
{
   if(filter.counter == filter.peak) filter.out_message << QString(str); return filter;
}

template<> OutputFilter& operator<<(OutputFilter& filter, QTextStream& (*control)(QTextStream &))
{
    if(filter.counter == filter.peak) filter.out_message << control; return filter;
}

