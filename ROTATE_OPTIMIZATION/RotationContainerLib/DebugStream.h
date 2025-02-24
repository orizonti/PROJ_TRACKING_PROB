#ifndef Q_DEBUG_H
#define Q_DEBUG_H

#include <iostream>
#include <streambuf>
#include <string>
#include <QDebug>

class QDebugStream : public std::basic_streambuf<char>
{
public:
	QDebugStream(std::ostream &stream) : m_stream(stream)
	{
		m_old_buf = stream.rdbuf();
		stream.rdbuf(this);
	}
	~QDebugStream()
	{
		// output anything that is left
		if (!m_string.empty())
			qDebug() << (m_string.c_str());

		m_stream.rdbuf(m_old_buf);
	}

protected:
	virtual int_type overflow(int_type v)
	{
		if (v == '\n')
		{
			qDebug() << (m_string.c_str());
			m_string.clear();
		}
		else
			m_string.push_back(v);

		return v;
	}

	virtual std::streamsize xsputn(const char *p, std::streamsize n)
	{
		m_string.append(p, p + n);

		int pos = 0;
		while (pos != std::string::npos)
		{
			pos = m_string.find('\n');
			if (pos != std::string::npos)
			{
				std::string tmp(m_string.begin(), m_string.begin() + pos);
				qDebug() << (tmp.c_str());
				m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
			}
		}

		return n;
	}

private:
	std::ostream &m_stream;
	std::streambuf *m_old_buf;
	std::string m_string;
};

#endif //Q_DEBUG_H
