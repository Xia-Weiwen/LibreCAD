#ifndef LC_ACTIONGETDIRECTION_H
#define LC_ACTIONGETDIRECTION_H

#include "rs_previewactioninterface.h"
#include "rs_line.h"
#include "rs_circle.h"
#include <QPointF>
#include "rs_debug.h"
#include "rs_dialogfactory.h"
#include "rs_coordinateevent.h"
#include "rs_preview.h"
#include "rs_graphicview.h"
#include "rs_pen.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>

/**
 * @brief The LC_ActionGetDirection class
 * This action class handle user events to select
 * one of the given directions from plugin
 *
 * @author Xia Weiwen
 * @date 2017/01/16 (16th Jan 2017)
 */

class LC_ActionGetDirection : public RS_PreviewActionInterface
{
    Q_OBJECT
public:
    LC_ActionGetDirection(RS_EntityContainer& container,
                          RS_GraphicView& graphicView,
                          QPointF* origin,
                          QList<double>* candidate_directions);
    ~LC_ActionGetDirection() override;

    void init(int status=0) override;
    void trigger() override;

    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void keyPressEvent(QKeyEvent* e);

    void coordinateEvent(RS_CoordinateEvent* e) override;
    void commandEvent(RS_CommandEvent* e) override;
    QStringList getAvailableCommands() override;

    void showOptions() override;
    void hideOptions() override;

    void updateMouseButtonHints() override;
    void updateMouseCursor() override;

    void setMessage(QString msg);
    bool isFinished() { return finished; }
    const double& getSelectedDirection() const {return _selected_dir; }

private:
    QPointF _origin;
    QList<double>* _candidates; // candidate directions
    double _selected_dir; // final selection
    QString _message; // tip for users
    double _arrow_length; // arrow to show direction
    RS_Pen _pen;

    double normalizeAngle(double raw_angle); // strict to [-PI, PI]
    double catchDirection(QMouseEvent* e);
    QList<RS_Line*>* createArrow(RS_Vector mouse); // create preview
    QList<RS_Line*>* createCandidateArrows(RS_Vector mouse);
    QList<RS_Line*>* createFreeArrow(RS_Vector mouse);
    int findNearestDirection(RS_Vector mouse);
};

#endif // LC_ACTIONGETDIRECTION_H
