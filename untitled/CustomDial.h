#ifndef CUSTOMDIAL_H
#define CUSTOMDIAL_H

#include <QDial>
#include <QString>
#include <QPen>

class CustomDial : public QDial
{
    Q_OBJECT

    Q_PROPERTY(QString arcColor READ getArcColor WRITE setArcColor)
    Q_PROPERTY(double arcWidth READ getArcWidth WRITE setArcWidth)
public:
    explicit CustomDial(QWidget* parent = nullptr);
    CustomDial(const QString& text,
               int minimum = 0,
               int maximum = 999,
               QWidget * parent = nullptr);
    ~CustomDial();

    void setArcColor(const QString& color);
    QString getArcColor() ;
    void setStartAngle(double angle);
    double getStartAngle() const;
    void setMaximumAngle(double angle);
    double getMaximumAngle() const;
    void setArcWidth(double px);
    double getArcWidth() const;
    void setText(const QString& text);
    QString getText() const;

private slots:
    void updateValue();

protected:
    virtual void paintEvent(QPaintEvent*) override;
    virtual void resizeEvent(QResizeEvent* event) override;
private:
    void init(const QString & text, int minimum, int maximum);

    double maximumAngleSpan_;
    double startAngle_;
    double arcWidth_;
    double angleSpan_;
    QString valueString_;
    QString text_;
    QRectF arcRect_;
    QRectF valueRect_;
    QRectF textRect_;
    QColor arcColor_;
    QPen arcPen_;
};
#endif
