#ifndef KEY_EVENT_ENGINE_H
#define KEY_EVENT_ENGINE_H
#include <QObject>
#include <QKeyEvent>

class KeyboardFilter : public QObject
{
    Q_OBJECT
public:
  static bool KeyControlPressed;
  static bool KeyAltPressed    ;

  static bool isControlPressed() { return KeyControlPressed; }
  static bool isAltPressed() { return KeyControlPressed; }
protected:

  bool eventFilter(QObject* obj, QEvent* event)
  {
      if(event->type() == QEvent::KeyPress) 
	  {
		QKeyEvent* key = static_cast<QKeyEvent*>(event);
		switch(key->key())
		{
		case Qt::Key_Control: KeyControlPressed = true; return true; 
		case Qt::Key_Alt:     KeyAltPressed     = true; return true; 
		default: return QObject::eventFilter(obj, event);
		}
	  }

      if(event->type() == QEvent::KeyRelease) 
	  {
			KeyControlPressed = false;
			KeyAltPressed     = false;
	  return QObject::eventFilter(obj, event);
	  }
  
      return false;
  }
};

#endif 
