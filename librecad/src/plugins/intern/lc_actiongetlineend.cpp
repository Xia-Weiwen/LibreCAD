#include "lc_actiongetlineend.h"

LC_ActionGetLineEnd::LC_ActionGetLineEnd(RS_EntityContainer &container,
                                         RS_GraphicView &graphicView,
                                         QPointF *start)
    :RS_PreviewActionInterface("Select Line End", container, graphicView)
{
    _start = start;
    _message = tr("Specify end point");
    _end = 0;
}

LC_ActionGetLineEnd::~LC_ActionGetLineEnd() = default;

void LC_ActionGetLineEnd::init(int status)
{
    RS_PreviewActionInterface::init(status);
}

void LC_ActionGetLineEnd::trigger()
{
    RS_PreviewActionInterface::trigger();
    finished = true;
    updateMouseButtonHints();
    finish();
}

void LC_ActionGetLineEnd::mouseMoveEvent(QMouseEvent *e)
{
    RS_Vector mouse = snapPoint(e);
    deletePreview();
    RS_Vector start_on_graph = graphicView->toGraph(
                graphicView->toGui(RS_Vector(_start->x(), _start->y())));
    RS_Vector mouse_on_graph = graphicView->toGraph(graphicView->toGui(mouse));
    preview->addEntity(new RS_Line(start_on_graph, mouse_on_graph));
    preview->setPen(RS_Pen(RS_Color(10, 255, 10), RS2::Width00, RS2::SolidLine));
    drawPreview();
}

void LC_ActionGetLineEnd::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button()==Qt::LeftButton) {
        RS_Vector snapped = snapPoint(e);

        RS_CoordinateEvent ce(snapped);
        coordinateEvent(&ce);
        _end = new QPointF(ce.getCoordinate().x, ce.getCoordinate().y);
        trigger();
    } else if (e->button()==Qt::RightButton) {
        deletePreview();
        finished = true;
        _end = 0;
        finish();
    }
}

void LC_ActionGetLineEnd::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
    {
        deletePreview();
        finished = true;
        _end = 0;
        finish();
    }
}

void LC_ActionGetLineEnd::coordinateEvent(RS_CoordinateEvent *e)
{
    Q_UNUSED(e)
}

void LC_ActionGetLineEnd::commandEvent(RS_CommandEvent *e)
{
    Q_UNUSED(e)
}

QStringList LC_ActionGetLineEnd::getAvailableCommands()
{
    return QStringList();
}

void LC_ActionGetLineEnd::updateMouseButtonHints()
{
    RS_DIALOGFACTORY->updateMouseWidget(_message, tr("Cancel"));
}

void LC_ActionGetLineEnd::showOptions()
{
    RS_ActionInterface::showOptions();

    RS_DIALOGFACTORY->requestOptions(this, true);
}

void LC_ActionGetLineEnd::hideOptions()
{
    RS_ActionInterface::hideOptions();
    RS_DIALOGFACTORY->requestOptions(this, false);
}

void LC_ActionGetLineEnd::updateMouseCursor()
{
    graphicView->setMouseCursor(RS2::CadCursor);
}

void LC_ActionGetLineEnd::setMessage(QString msg)
{
    _message = msg;
}

