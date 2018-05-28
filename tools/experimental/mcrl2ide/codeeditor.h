#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

class LineNumberArea;

/**
 * @brief The CodeEditor class defines a text editor for code (used for specification and properties)
 */
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = 0);

    /**
     * @brief lineNumberAreaPaintEvent paints the line number area on the screen
     * @param event a paint event
     */
    void lineNumberAreaPaintEvent(QPaintEvent *event);

    /**
     * @brief lineNumberAreaWidth computes the width needed for the line number area
     * @return the width needed for the line number area
     */
    int lineNumberAreaWidth();

protected:
    /**
     * @brief resizeEvent resizes the line number area when the window is resized
     * @param e the resize event
     */
    void resizeEvent(QResizeEvent *event) override;

private slots:
    /**
     * @brief updateLineNumberAreaWidth updates the width of the line number area
     */
    void updateLineNumberAreaWidth(int);

    /**
     * @brief updateLineNumberArea updates the line number area after the scrollbar has been used
     * @param rect the rectangle that covers the line number area
     * @param dy the amount of pixels scrolled
     */
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;
};


/**
 * @brief The LineNumberArea class defines the area with line numbers in the code editor
 */
class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor);

    /**
     * @brief sizeHint returns the recommended size of the widget
     * @return the recommended size of the widget
     */
    QSize sizeHint() const override;

protected:
    /**
     * @brief paintEvent handles paint events
     * @param event a paint event
     */
    void paintEvent(QPaintEvent *event) override;

private:
    CodeEditor *codeEditor;
};

#endif // CODEEDITOR_H
