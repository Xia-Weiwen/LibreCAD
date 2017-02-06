#ifndef LC_ACTIONGETLINEEND_H
#define LC_ACTIONGETLINEEND_H

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
 * @brief The LC_ActionGetLineEnd class
 * This action class handle user events to select
 * end point of a line with preview from plugin
 *
 * @author Xia Weiwen
 * @date 2017/02/06
 */

class LC_ActionGetLineEnd : public RS_PreviewActionInterface
{
    Q_OBJECT
public:
    LC_ActionGetLineEnd(RS_EntityContainer &container,
                        RS_GraphicView &graphicView,
                        QPointF* start);
    ~LC_ActionGetLineEnd() override;

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

    QPointF* getEndPoint() { return _end; }

private:
    QPointF* _start;
    QPointF* _end;
    QString _message;
};

#endif // LC_ACTIONGETLINEEND_H
