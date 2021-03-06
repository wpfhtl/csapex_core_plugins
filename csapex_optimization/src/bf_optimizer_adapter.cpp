/// HEADER
#include "bf_optimizer_adapter.h"

/// COMPONENT
#include <csapex_optimization/parameter_dialog.h>

/// PROJECT
#include <csapex/view/utility/register_node_adapter.h>
#include <csapex/param/range_parameter.h>

/// SYSTEM
#include <QPushButton>
#include <QDialog>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QFormLayout>
#include <QProgressBar>

using namespace csapex;

CSAPEX_REGISTER_NODE_ADAPTER(BFOptimizerAdapter, csapex::BFOptimizer)


BFOptimizerAdapter::BFOptimizerAdapter(NodeHandleWeakPtr worker, NodeBox* parent, std::weak_ptr<BFOptimizer> node)
    : DefaultNodeAdapter(worker, parent), wrapped_(node)
{
    auto n = wrapped_.lock();
    trackConnection(n->step.connect(std::bind(&BFOptimizerAdapter::triggerStep, this, std::placeholders::_1)));
}

BFOptimizerAdapter::~BFOptimizerAdapter()
{
}

void BFOptimizerAdapter::setupUi(QBoxLayout* layout)
{
    DefaultNodeAdapter::setupUi(layout);

    QPushButton* btn_add_param = new QPushButton("+");
    layout->addWidget(btn_add_param);
    QObject::connect(btn_add_param, SIGNAL(clicked()), this, SLOT(createParameter()));

    progress_ = new QProgressBar;
    layout->addWidget(progress_);
}

void BFOptimizerAdapter::startOptimization()
{
    auto node = wrapped_.lock();
    if(!node) {
        return;
    }
    node->start();
}


void BFOptimizerAdapter::stopOptimization()
{
    auto node = wrapped_.lock();
    if(!node) {
        return;
    }
    node->stop();
}

QDialog* BFOptimizerAdapter::makeTypeDialog()
{
    QVBoxLayout* layout = new QVBoxLayout;

    QFormLayout* form = new QFormLayout;

    QComboBox* type = new QComboBox;
    type->addItem("double");
    form->addRow("Type", type);

    QObject::connect(type, SIGNAL(currentIndexChanged(QString)), this, SLOT(setNextParameterType(QString)));
    next_type_ = type->currentText().toStdString();

    layout->addLayout(form);

    QDialogButtonBox* buttons = new QDialogButtonBox;
    buttons->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttons);


    QDialog* dialog = new QDialog;
    dialog->setWindowTitle("Create Parameter");
    dialog->setLayout(layout);
    dialog->setModal(true);

    QObject::connect(buttons, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttons, SIGNAL(rejected()), dialog, SLOT(reject()));

    return dialog;
}

void BFOptimizerAdapter::setNextParameterType(const QString &type)
{
    next_type_ = type.toStdString();
}


void BFOptimizerAdapter::createParameter()
{
    auto node = wrapped_.lock();
    if(!node) {
        return;
    }
    QDialog* type_dialog = makeTypeDialog();
    if(type_dialog->exec() == QDialog::Accepted) {

        ParameterDialog diag(next_type_);
        if(diag.exec() == QDialog::Accepted) {
            csapex::param::Parameter::Ptr param = diag.getParameter();
            node->addPersistentParameter(param);

            progress_->setMaximum(node->stepsNecessary());

            node->restart();
        }
    }
}

void BFOptimizerAdapter::triggerStep(int s)
{
    auto node = wrapped_.lock();
    if(!node) {
        return;
    }
    progress_->setValue(s);
    progress_->setMaximum(node->stepsNecessary());
}
/// MOC
#include "moc_bf_optimizer_adapter.cpp"
