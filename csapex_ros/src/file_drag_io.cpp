/// COMPONENT
#include <csapex_ros/ros_handler.h>

/// PROJECT
#include <csapex/view/designer/drag_io_handler.h>
#include <csapex/utility/uuid.h>
#include <csapex/command/dispatcher.h>
#include <csapex/command/add_node.h>
#include <csapex/model/graph.h>
#include <csapex/model/generic_state.h>
#include <csapex/model/node_state.h>
#include <csapex/param/parameter_factory.h>
#include <csapex/utility/register_apex_plugin.h>
#include <csapex/view/designer/graph_view.h>
#include <csapex/model/graph_facade.h>

/// SYSTEM
#include <QStringList>
#include <QUrl>
#include <QMimeData>
#include <boost/regex.hpp>

namespace csapex
{

class RosIoHandler : public DragIOHandler
{
    static const boost::regex fmt;

    std::string getCmd(QDropEvent* e)
    {
        QByteArray itemData = e->mimeData()->data("application/x-qabstractitemmodeldatalist");
        QDataStream stream(&itemData, QIODevice::ReadOnly);

        int r, c;
        QMap<int, QVariant> v;
        stream >> r >> c >> v;

        return v[Qt::UserRole].toString().toStdString();
    }

    virtual bool handleEnter(GraphView* view, CommandDispatcher* dispatcher, QDragEnterEvent* e) {
        if(e->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
            std::string cmd = getCmd(e);

            bool cmd_could_be_topic = boost::regex_match(cmd, fmt);

            if(cmd_could_be_topic) {
                if(ROSHandler::instance().isConnected() && ROSHandler::instance().topicExists(cmd)) {
                    e->accept();
                    return true;
                }
            }
        }
        return false;
    }
    virtual bool handleMove(GraphView* view, CommandDispatcher* dispatcher, QDragMoveEvent* e){
        return false;
    }
    virtual bool handleDrop(GraphView* view, CommandDispatcher* dispatcher, QDropEvent* e, const QPointF& scene_pos) {
        if(e->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist")) {
            std::string cmd = getCmd(e);

            bool cmd_could_be_topic = boost::regex_match(cmd, fmt);

            if(cmd_could_be_topic) {
                if(ROSHandler::instance().topicExists(cmd)) {
                    QPoint pos = e->pos();

                    GraphFacade* gf = view->getGraphFacade();
                    Graph* graph = gf->getGraph();
                    std::string type("csapex::ImportRos");
                    UUID uuid = graph->generateUUID(type);

                    NodeState::Ptr state(new NodeState(nullptr));

                    GenericState::Ptr child_state(new GenericState);
                    child_state->addParameter(csapex::param::ParameterFactory::declareFileInputPath("topic", cmd));
                    state->setParameterState(child_state);

                    dispatcher->execute(Command::Ptr(new command::AddNode(gf->getAbsoluteUUID(), type, Point(pos.x(), pos.y()), uuid, state)));

                    return true;
                }
            }
        }
        return false;
    }
};

const boost::regex RosIoHandler::fmt("[a-zA-Z0-9_\\-/]+");
}

CSAPEX_REGISTER_CLASS(csapex::RosIoHandler, csapex::DragIOHandler)
