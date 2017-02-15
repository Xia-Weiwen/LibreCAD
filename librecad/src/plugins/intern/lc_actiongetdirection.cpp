#include "lc_actiongetdirection.h"

LC_ActionGetDirection::LC_ActionGetDirection(RS_EntityContainer &container,
                                             RS_GraphicView &graphicView,
                                             QPointF *origin,
                                             QList<double> *candidate_directions)
    :RS_PreviewActionInterface("Select Direction", container, graphicView)
{
    _origin = *origin;
    _candidates = new QList<double>;
    for (double a : *candidate_directions)
    {
        _candidates->append(normalizeAngle(a));
    }
    _message = tr("Select direction");
    finished = false;
    _arrow_length = 120.0;
    _pen = RS_Pen(RS_Color(10, 255, 10), RS2::Width00, RS2::SolidLine);
    _selected_dir = -2 * M_PI;
}

LC_ActionGetDirection::~LC_ActionGetDirection() = default;

void LC_ActionGetDirection::init(int status)
{
    RS_PreviewActionInterface::init(status);
}

void LC_ActionGetDirection::trigger()
{
    RS_PreviewActionInterface::trigger();
    finished = true;
    updateMouseButtonHints();
    finish();
}

void LC_ActionGetDirection::mouseMoveEvent(QMouseEvent *e)
{
//    RS_Vector mouse = snapPoint(e);
    RS_Vector mouse = RS_Vector(graphicView->toGraphX(e->x()),
                                graphicView->toGraphY(e->y()));
    deletePreview();
    QList<RS_Line*>* arrowLines = createArrow(mouse);
    for (RS_Line* line : *arrowLines)
    {
        preview->addEntity(line);
    }
    RS_Vector origin_on_screen = RS_Vector{graphicView->toGuiX(_origin.x()),
            graphicView->toGuiY(_origin.y())};
    RS_Vector* origin_on_graph = new RS_Vector(graphicView->toGraph(origin_on_screen));
    RS_Vector offset_on_screan = origin_on_screen + RS_Vector(2, 0);
    RS_Vector offset_on_graph = graphicView->toGraph(offset_on_screan);
    double radius = hypot(offset_on_graph.x - origin_on_graph->x,
                          offset_on_graph.y - origin_on_graph->y);
    RS_CircleData* originData = new RS_CircleData{*origin_on_graph, radius};
    preview->addEntity(new RS_Circle{0, *originData});
    preview->setPen(_pen);
    drawPreview();
}

void LC_ActionGetDirection::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button()==Qt::LeftButton) {
        RS_Vector snapped = snapPoint(e);

        RS_CoordinateEvent ce(snapped);
        coordinateEvent(&ce);
//        _selected_dir = catchDirection(e);
        trigger();
    } else if (e->button()==Qt::RightButton) {
        deletePreview();
        finished = true;
        _selected_dir = -2 * M_PI;
        finish();
    }
}

void LC_ActionGetDirection::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
    {
        deletePreview();
        finished = true;
        _selected_dir = -2 * M_PI;
        finish();
    }
}

void LC_ActionGetDirection::coordinateEvent(RS_CoordinateEvent *e)
{
    Q_UNUSED(e)
}

void LC_ActionGetDirection::commandEvent(RS_CommandEvent *e)
{
    Q_UNUSED(e)
}

QStringList LC_ActionGetDirection::getAvailableCommands()
{
    return QStringList();
}

void LC_ActionGetDirection::updateMouseButtonHints()
{
    RS_DIALOGFACTORY->updateMouseWidget(_message, tr("Cancel"));
}

void LC_ActionGetDirection::showOptions()
{
    RS_DEBUG->print("LC_ActionGetDirection::showOptions");
    RS_ActionInterface::showOptions();

    RS_DIALOGFACTORY->requestOptions(this, true);
    RS_DEBUG->print("LC_ActionGetDirection::showOptions: OK");
}

void LC_ActionGetDirection::hideOptions()
{
    RS_ActionInterface::hideOptions();
    RS_DIALOGFACTORY->requestOptions(this, false);
}

void LC_ActionGetDirection::updateMouseCursor()
{
    graphicView->setMouseCursor(RS2::CadCursor);
}

void LC_ActionGetDirection::setMessage(QString msg)
{
    _message = msg;
}

double LC_ActionGetDirection::catchDirection(QMouseEvent *e)
{
    Q_UNUSED(e)
    return 0.0;
}

QList<RS_Line*>* LC_ActionGetDirection::createArrow(RS_Vector mouse)
{
    // no candidates, draw free arrow
    if (_candidates->size() == 0)
        return createFreeArrow(mouse);
    return createCandidateArrows(mouse);
}

QList<RS_Line*>* LC_ActionGetDirection::createFreeArrow(RS_Vector mouse)
{
    double angle = atan2(mouse.y - _origin.y(), mouse.x - _origin.x());
    RS_Vector origin_on_screen = RS_Vector{graphicView->toGuiX(_origin.x()),
            graphicView->toGuiY(_origin.y())};
    RS_Vector* top_on_screen = new RS_Vector(_arrow_length * cos(angle) + origin_on_screen.x,
                                   -_arrow_length * sin(angle) + origin_on_screen.y);
    RS_Vector* top_on_graph = new RS_Vector(graphicView->toGraph(*top_on_screen));
    RS_Vector* _arrowhead_1 = new RS_Vector(_arrow_length / 5 * cos(angle + 3 * M_PI_4) + top_on_screen->x,
                                            -_arrow_length / 5 * sin(angle + 3 * M_PI_4) + top_on_screen->y);
    *_arrowhead_1 = graphicView->toGraph(*_arrowhead_1);
    RS_Vector* _arrowhead_2 = new RS_Vector(_arrow_length / 5 * cos(angle - 3 * M_PI_4) + top_on_screen->x,
                                            -_arrow_length / 5 * sin(angle - 3 * M_PI_4) + top_on_screen->y);
    *_arrowhead_2 = graphicView->toGraph(*_arrowhead_2);
    RS_Line* shaft = new RS_Line(RS_Vector(_origin.x(), _origin.y()), *top_on_graph);
    RS_Line* head_1 = new RS_Line(*top_on_graph, *_arrowhead_1);
    RS_Line* head_2 = new RS_Line(*top_on_graph, *_arrowhead_2);
    QList<RS_Line*>* arrowLines = new QList<RS_Line*>;
    *arrowLines << shaft << head_1 << head_2;
    _selected_dir = angle; // the selected angle is always updated
    return arrowLines;
}

QList<RS_Line*>* LC_ActionGetDirection::createCandidateArrows(RS_Vector mouse)
{
    QList<RS_Line*>* arrowLines = new QList<RS_Line*>;
    int selection_index = findNearestDirection(mouse);
    for (int i = 0; i < _candidates->size(); ++i)
    {
        double l_factor = i == selection_index ? 1 : 0.5;
        double angle = _candidates->at(i);
        RS_Vector origin_on_screen = RS_Vector{graphicView->toGuiX(_origin.x()),
                graphicView->toGuiY(_origin.y())};
        RS_Vector* top_on_screen =
                new RS_Vector(_arrow_length * l_factor * cos(angle) + origin_on_screen.x,
                              -_arrow_length * l_factor * sin(angle) + origin_on_screen.y);
        RS_Vector* top_on_graph = new RS_Vector(graphicView->toGraph(*top_on_screen));
        RS_Vector* _arrowhead_1 =
                new RS_Vector(_arrow_length * l_factor / 5 * cos(angle + 3 * M_PI_4) + top_on_screen->x,
                              -_arrow_length * l_factor / 5 * sin(angle + 3 * M_PI_4) + top_on_screen->y);
        *_arrowhead_1 = graphicView->toGraph(*_arrowhead_1);
        RS_Vector* _arrowhead_2 =
                new RS_Vector(_arrow_length * l_factor / 5 * cos(angle - 3 * M_PI_4) + top_on_screen->x,
                              -_arrow_length * l_factor / 5 * sin(angle - 3 * M_PI_4) + top_on_screen->y);
        *_arrowhead_2 = graphicView->toGraph(*_arrowhead_2);
        RS_Line* shaft = new RS_Line(RS_Vector(_origin.x(), _origin.y()), *top_on_graph);
        RS_Line* head_1 = new RS_Line(*top_on_graph, *_arrowhead_1);
        RS_Line* head_2 = new RS_Line(*top_on_graph, *_arrowhead_2);
        *arrowLines << shaft << head_1 << head_2;
    }
    _selected_dir = _candidates->at(selection_index); // the selected angle is always updated
    return arrowLines;
}

double LC_ActionGetDirection::normalizeAngle(double raw_angle)
{
    double angle = raw_angle;
    while (angle >= M_PI)
    {
        angle -= 2 * M_PI;
    }
    while (angle <= -M_PI)
    {
        angle += 2 * M_PI;
    }
    return angle;
}

/**
 * @brief LC_ActionGetDirection::findNearestDirection
 * @param mouse
 * @return index in the _candidates list
 */
int LC_ActionGetDirection::findNearestDirection(RS_Vector mouse)
{
    double min_diff = M_PI;
    int min_index = 0;
    // compare this angle with candidates, choose the nearest one
    double mouse_angle = atan2(mouse.y - _origin.y(), mouse.x - _origin.x());
    for (int i = 0; i < _candidates->size(); ++i)
    {
        double diff = fabs(mouse_angle - _candidates->at(i));
        if (diff > M_PI) diff = 2 * M_PI - diff;
        if (diff < min_diff)
        {
            min_diff = diff;
            min_index = i;
        }
    }
    return min_index;
}
