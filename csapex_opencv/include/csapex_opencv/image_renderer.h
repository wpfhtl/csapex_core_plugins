#ifndef IMAGE_RENDERER_H
#define IMAGE_RENDERER_H

/// PROJECT
#include <csapex/view/message_renderer.h>

/// COMPONENT
#include <csapex_opencv/cv_mat_message.h>

namespace csapex
{
class CSAPEX_EXPORT_PLUGIN ImageRenderer : public MessageRendererImplementation<connection_types::CvMatMessage>
{
public:
    virtual QImage doRender(const connection_types::CvMatMessage& msg);
};
}

#endif // IMAGE_RENDERER_H

