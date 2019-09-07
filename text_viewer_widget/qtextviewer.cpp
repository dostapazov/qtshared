#include "qtextviewer.h"
#include "qpainter.h"
#include <qdebug.h>

#include <utility>


QTextViewer::QTextViewer(QWidget *parent)
    : QWidget(parent)
    //,m_mutex(QMutex::Recursive)
{
  set_lines_limit(1024);
  connect(&repaint_timer,SIGNAL(timeout()),this, SLOT(repaint()));
  repaint_timer.setSingleShot(true);
}

QTextViewer::~QTextViewer()
{
   remove_lines(0,this->m_lines.size());
}

void     QTextViewer::setFont(const QFont & font)
{
    QWidget::setFont(font);
    m_font_height = 0;
    update_ystart();
}


void     QTextViewer::line_add    (const QString& text, QColor text_color, bool scroll_to_end)
{
   size_t curr_size = m_lines.size();
   //QMutexLocker ml(&m_mutex);
   if(!(curr_size < m_lines_limit))
       remove_lines();
   m_lines.push_back(new _text_line_t(text, text_color, Qt::white));
   if(scroll_to_end) update_ystart();
   start_repaint();
}

void     QTextViewer::update_ystart()
{
    int max_view_line = rect().height() / (this->m_vspace+get_font_height());
    m_ystart = (max_view_line < int(m_lines.size())) ? int(m_lines.size()) - max_view_line : 0;
}

QSize QTextViewer::sizeHint() const
{
  QFontMetrics fm (this->font());
  QSize ret = rect().size();
  ret.setHeight(int(m_lines.size()) * (m_vspace + m_font_height));

  return ret;
}


int     QTextViewer::remove_lines(size_t start , size_t count )
{
  if(start < m_lines.size() )
  {
     //QMutexLocker ml(&m_mutex);
     count = qMin(count,m_lines.size()-start);
     auto bptr =  m_lines.begin();
     std::advance(bptr,start);
     auto eptr = bptr;
     std::advance(eptr,count);
     auto ptr = bptr;
     while(ptr< eptr)
        {
         delete *ptr;
         ++ptr;
        }

     m_lines.erase(bptr, eptr);
     start_repaint();
     return int(count);
  }
  return 0;
}

void     QTextViewer::set_lines_limit(size_t new_limit)
{
  if(m_lines_limit!=new_limit)
  {

    if(m_lines_limit>new_limit)
       remove_lines(0 ,m_lines_limit - new_limit );
    m_lines_limit = new_limit;
    start_repaint();
  }
}

void    QTextViewer::clear()
{
  remove_lines(0,m_lines.size());
  update_ystart();
}

int     QTextViewer::get_font_height()
{
    if(!m_font_height)
       m_font_height = QFontMetrics(this->font()).height();
    return m_font_height;
}

int  QTextViewer::get_line_v_offset (int line_num)
{
    get_font_height();
    if(line_num<this->m_ystart)
        return -m_vspace - m_font_height;
    return  m_vspace + (m_font_height+m_vspace) * (line_num-m_ystart);

}

void QTextViewer::start_repaint()
{
  if(!repaint_timer.isActive())
  {
     repaint_timer.start(m_update_time);
  }
}

void     QTextViewer::paintEvent(QPaintEvent* paint_event)
{
  Q_UNUSED(paint_event);
  m_max_len =  0;
  repaint_timer.stop();

  get_font_height();
  QPainter painter(this);
  QBrush br(QWidget::palette().color(QWidget::backgroundRole()));
  QRect r = rect();
  painter.fillRect(r,br);

  //QMutexLocker ml(&m_mutex);
  auto ptr = m_lines.begin(), end = m_lines.end();
  std::advance(ptr,this->m_ystart);
  int y_pos = m_vspace + m_font_height;
  QColor c = painter.pen().color();
  while(ptr<end && y_pos < r.height())
  {
   text_lines_t::value_type v = *ptr;
   if(c!=v->m_fcolor)
   {
       c=v->m_fcolor;
       painter.setPen(c);
   }
   m_max_len = qMax(m_max_len, v->m_text.length());
   if(m_xstart)
      painter.drawText(0,y_pos,v->m_text.mid(m_xstart));
     else
      painter.drawText(0,y_pos,v->m_text);

   y_pos += (m_vspace + m_font_height);
   ++ptr;
  }
  update_sbars();
}

#ifndef QT_NO_WHEELEVENT
void QTextViewer::wheelEvent(QWheelEvent *event)
{
  if(!this->m_lines.empty() && event->delta())
  {
    m_ystart -= event->delta() > 0 ? 1:-1;
    m_ystart = qMin(qMax(m_ystart, 0), int(m_lines.size())-1);
    start_repaint();
  }
}
#endif


void     QTextViewer::set_scrollbars(QScrollBar* vsb,QScrollBar* hsb)
{
   if(m_vsbar != vsb)
   {
    if(m_vsbar)   m_vsbar->disconnect();

    m_vsbar = vsb;
    if(vsb)
      connect(vsb, SIGNAL(valueChanged(int)), this, SLOT(vsbar_changed(int)));

   }

   if(m_hsbar != hsb)
   {
    if(m_hsbar)   m_hsbar->disconnect();

    m_hsbar = hsb;
    if(hsb)
      connect(hsb, SIGNAL(valueChanged(int)), this, SLOT(hsbar_changed(int)));
   }
   update_sbars();
}

void QTextViewer::update_sbars  ()
{
  if(m_vsbar)
  {
      QSignalBlocker sb(m_vsbar);
      int vmax = qMax(0,int(m_lines.size()) -1);
      if( m_vsbar->isVisible() != bool(vmax))
            m_vsbar->setVisible(vmax != 0);
      if(vmax !=m_vsbar->maximum())
          m_vsbar->setRange(0,vmax);
      if(m_vsbar->value() != m_ystart)
          m_vsbar->setValue(m_ystart);
  }

  if(m_hsbar)
  {
    QSignalBlocker bl(m_hsbar);
    int hmax = qMax(0, m_max_len -1 );
    if( m_hsbar->isVisible() != bool(hmax))
          m_hsbar->setVisible(hmax != 0);

    if(hmax !=m_hsbar->maximum())
        m_hsbar->setRange(0,hmax);
    if(m_hsbar->value() != m_xstart)
        m_hsbar->setValue(m_xstart);
  }
}

void QTextViewer::vsbar_changed(int pos)
{
  if(m_ystart != pos)
  {
    m_ystart = pos;
    start_repaint();
  }
}

void QTextViewer::hsbar_changed(int pos)
{
  if(m_xstart != pos)
  {
    m_xstart = pos;
    start_repaint();
  }
}






