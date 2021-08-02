ifndef CUSTOMDIAL_H
#define CUSTOMDIAL_H
#include <QDial>

class CustomDial : public QDial
{
    Q_OBJECT

    Q_PROPERTY(double knobRadius READ getKnobRadius WRITE setKnobRadius)

    Q_PROPERTY(double knobMargin READ getKnobMargin WRITE setKnobMargin)

public:

    CustomDial(QWidget * parent = nullptr,
               double knobRadius = 5,
               double knobMargin = 5);

    void setKnobRadius(double radius);

    double getKnobRadius() const;

    void setKnobMargin(double margin);

    double getKnobMargin() const;

private:

    virtual void paintEvent(QPaintEvent*) override;

    double knobRadius_;

    double knobMargin_;
};
#endif
