/// HEADER
#include "output_display_adapter.h"

/// PROJECT
#include <csapex/msg/io.h>
#include <csapex/view/utility/register_node_adapter.h>
#include <csapex/utility/assert.h>

/// SYSTEM
#include <QPainter>
#include <QGraphicsSceneEvent>
#include <QGraphicsPixmapItem>
#include <QCheckBox>
#include <QPushButton>
#include <QBoxLayout>
#include <QResizeEvent>

using namespace csapex;

CSAPEX_REGISTER_NODE_ADAPTER(OutputDisplayAdapter, csapex::OutputDisplay)


OutputDisplayAdapter::OutputDisplayAdapter(NodeHandleWeakPtr worker, NodeBox* parent, std::weak_ptr<OutputDisplay> node)
    : DefaultNodeAdapter(worker, parent), wrapped_(node)
{
    auto n = wrapped_.lock();

    // translate to UI thread via Qt signal
    trackConnection(n->display_request.connect(std::bind(&OutputDisplayAdapter::displayRequest, this, std::placeholders::_1)));

    n->setAdapted();
}

OutputDisplayAdapter::~OutputDisplayAdapter()
{

}

bool OutputDisplayAdapter::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::Resize){
        QSize s = label_view_->sizeHint();
        state.width = s.width();
        state.height = s.height();
    }

    return false;
}

void OutputDisplayAdapter::resize()
{
    label_view_->setSize(state.width, state.height);
}


void OutputDisplayAdapter::setupUi(QBoxLayout* layout)
{
    label_view_ = new ImageWidget;
    label_view_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    label_view_->installEventFilter(this);

    layout->addWidget(label_view_);

    QHBoxLayout* sub = new QHBoxLayout;

    QPushButton* fit = new QPushButton("resize to content");
    sub->addWidget(fit, 0,  Qt::AlignLeft);
    QObject::connect(fit, SIGNAL(clicked()), this, SLOT(fitInView()));

    sub->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    layout->addLayout(sub);

    connect(this, SIGNAL(displayRequest(QImage)), this, SLOT(display(QImage)));

    DefaultNodeAdapter::setupUi(layout);
}

void OutputDisplayAdapter::setManualResize(bool manual)
{
    label_view_->setManualResize(manual);
}

bool OutputDisplayAdapter::isResizable() const
{
    return true;
}

void OutputDisplayAdapter::fitInView()
{
    state.width = last_size_.width();
    state.height = last_size_.height();

    resize();
}

Memento::Ptr OutputDisplayAdapter::getState() const
{
    return std::shared_ptr<State>(new State(state));
}

void OutputDisplayAdapter::setParameterState(Memento::Ptr memento)
{
    std::shared_ptr<State> m = std::dynamic_pointer_cast<State> (memento);
    apex_assert(m.get());

    state = *m;

    resize();
}

void OutputDisplayAdapter::display(const QImage& img)
{
    last_size_ = img.size();
    label_view_->setPixmap(QPixmap::fromImage(img));
    label_view_->repaint();
}







ImageWidget::ImageWidget(QWidget *parent) :
    QWidget(parent), size(100,100), manual_resize_(false)
{
}

void ImageWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    if (pix.isNull())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QSize pixSize = pix.size();
    pixSize.scale(event->rect().size(), Qt::KeepAspectRatio);

    QPixmap scaledPix = pix.scaled(pixSize,
                                   Qt::KeepAspectRatio,
                                   Qt::SmoothTransformation
                                   );

    int x = (width() - scaledPix.width()) / 2;
    int y = (height() - scaledPix.height()) / 2;

    painter.drawPixmap(QPoint(x,y), scaledPix);

}

void ImageWidget::resizeEvent(QResizeEvent * re)
{
    if(manual_resize_) {
        setSize(re->size());
    }
}

const QPixmap* ImageWidget::pixmap() const
{
    return &pix;
}

void ImageWidget::setPixmap (const QPixmap &pixmap)
{
    pix = pixmap;
}

void ImageWidget::setManualResize(bool manual)
{
    manual_resize_ = manual;
    updateGeometry();
}

void ImageWidget::setSize(const QSize &s)
{
    size = s;
    updateGeometry();
}
void ImageWidget::setSize(int w, int h)
{
    setSize(QSize(w,h));
}

QSize ImageWidget::sizeHint() const
{
    return size;
}
QSize ImageWidget::minimumSizeHint() const
{
    if(manual_resize_) {
        return QSize(10, 10);
    } else {
        return size;
    }
}


/// MOC
#include "../moc_output_display_adapter.cpp"

