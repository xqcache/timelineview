#pragma once

#include <QWidget>

struct WaitingMaskViewPrivate;

class WaitingMaskView : public QWidget {
    Q_OBJECT
public:
    explicit WaitingMaskView(QWidget* parent = nullptr);
    ~WaitingMaskView() noexcept override;

    void setText(const QString& text);
    void setTask(const std::function<void()>& task);
    void exec();

private:
    void initUi();

private:
    WaitingMaskViewPrivate* d_ { nullptr };
};