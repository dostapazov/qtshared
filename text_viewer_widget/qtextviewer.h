#ifndef QTEXTVIEWER_H
#define QTEXTVIEWER_H

#include <QWidget>
#include "qevent.h"
#include <qtimer.h>
#include <QScrollBar>
#include <qmutex.h>

struct _text_line_t
{
    QColor  m_bcolor;
    QColor  m_fcolor;
    QString m_text;
    _text_line_t(const QString& _text, QColor text_c, QColor bkc);
    _text_line_t(const _text_line_t& other);
    _text_line_t& operator = (const _text_line_t& other);

};

class QTextViewer : public QWidget
{
    Q_OBJECT
    QTimer        repaint_timer;
    typedef std::vector<_text_line_t*> text_lines_t;
    text_lines_t  m_lines;
    size_t        m_lines_limit = 1024;
    int           m_max_len    = 0; // Максимальная длина строки в отрисованном frame
    int           m_xmargin    = 6;
    int           m_ymargin    = 6;
    int           m_xstart     = 0;
    int           m_ystart     = 0;
    int           m_font_height = 0;
    int           m_vspace      = 2;//space between lines;
    int           m_update_time = 20;
    QScrollBar*   m_vsbar       = Q_NULLPTR;
    QScrollBar*   m_hsbar       = Q_NULLPTR;
//    QMutex        m_mutex;

    virtual void paintEvent(QPaintEvent* paint_event) override;
#ifndef QT_NO_WHEELEVENT
    virtual void wheelEvent(QWheelEvent* event) override;
#endif
    int  get_font_height();
    int  get_line_v_offset (int line_num);
    void start_repaint();
    void update_ystart();
    void update_sbars  ();

public:
    explicit QTextViewer(QWidget* parent = nullptr);
    ~QTextViewer() override;
    int      remove_lines(size_t start = 0, size_t count = 1);
    void     set_lines_limit(size_t new_limit);
    size_t   lines_limit    ();
    void     set_update_time(int value) {m_update_time = value;}
    void     setFont(const QFont& font);

    void     set_scrollbars(QScrollBar* vsb, QScrollBar* hsb);


    QScrollBar* vscorllbar    () ;
    virtual QSize sizeHint() const override;
public slots:
    void     clear();
    void     line_add    (const QString& text, QColor text_color = Qt::black, bool scroll_to_end = true );
private slots:
    void vsbar_changed(int pos);
    void hsbar_changed(int pos);

};

inline _text_line_t::_text_line_t(const QString& _text, QColor text_c, QColor bkc)
    : m_bcolor(bkc)
    , m_fcolor(text_c)
    , m_text  (_text)
{}

inline _text_line_t::_text_line_t(const _text_line_t& other)
{
    *this = other;
}

inline _text_line_t& _text_line_t::operator = (const _text_line_t& other)
{
    this->m_bcolor = other.m_bcolor;
    this->m_fcolor = other.m_fcolor;
    this->m_text   = other.m_text;
    return *this;
}

inline size_t   QTextViewer::lines_limit    () {return m_lines_limit;}

inline QScrollBar* QTextViewer::vscorllbar () {return m_vsbar;}



#endif // QTEXTVIEWER_H
