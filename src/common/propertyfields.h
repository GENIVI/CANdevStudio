#ifndef __PROPERTYFIELDS_H
#define __PROPERTYFIELDS_H

#include <QColorDialog>
#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLineEdit>
#include <QPushButton>

struct PropertyField : public QWidget {
    virtual void setPropText(const QString& text) = 0;
    virtual QString propText() = 0;
};

struct PropertyFieldText : public PropertyField {
    PropertyFieldText(bool numberOnly = false)
        : _numberOnly(numberOnly)
    {
        setLayout(new QHBoxLayout);
        layout()->setContentsMargins(0, 0, 0, 0);
        _le = new QLineEdit(this);
        _le->setFrame(false);
        layout()->addWidget(_le);

        if (_numberOnly) {
            _le->setValidator(new QIntValidator(_le));
        }
    }

    void setPropText(const QString& text) override
    {
        _le->setText(text);
    }

    QString propText() override
    {
        return _le->text();
    }

protected:
    QLineEdit* _le;

private:
    const bool _numberOnly;
};

struct PropertyFieldPath : public PropertyFieldText {
    PropertyFieldPath(bool folderOnly = false)
        : _folderOnly(folderOnly)
    {
        _pb = new QPushButton(this);
        _pb->setText("...");
        _pb->setFixedSize(24, 24);
        _pb->setFlat(true);
        _pb->setProperty("type", "PropertyFieldPath");
        layout()->addWidget(_pb);

        connect(_pb, &QPushButton::pressed, [&] {
            QString path;
            QString currPath = QFileInfo(_le->text()).path();

            if (currPath.length() == 0) {
                currPath = QDir::homePath();
            }

            if (_folderOnly) {
                path = QFileDialog::getExistingDirectory(
                    this, "Select directory", currPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
            } else {
                path = QFileDialog::getOpenFileName(this, "Select file", currPath);
            }

            if (path.length() > 0) {
                _le->setText(path);
            }
        });
    }

private:
    QPushButton* _pb;
    const bool _folderOnly;
};

class PropertyFieldCombo : public PropertyField {
    Q_OBJECT

public:
    PropertyFieldCombo(bool editable = true)
    {
        setLayout(new QHBoxLayout);
        layout()->setContentsMargins(0, 0, 0, 0);
        _cb = new QComboBox(this);
        _cb->setEditable(editable);
        _cb->setFrame(false);
        layout()->addWidget(_cb);
        connect(_cb, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this,
            &PropertyFieldCombo::currentTextChanged);
    }

    void setPropText(const QString& text) override
    {
        _cb->setCurrentText(text);
    }

    QString propText() override
    {
        QVariant data = _cb->currentData();

        if (data.isValid()) {
            return data.toString();
        } else {
            return _cb->currentText();
        }
    }

    void addItems(const QStringList& list)
    {
        _cb->clear();
        _cb->addItems(list);
    }

    void addItem(const QString& text, const QVariant v = QVariant())
    {
        _cb->addItem(text, v);
    }

    void sort()
    {
        _cb->model()->sort(0);
    }

signals:
    void currentTextChanged(const QString& text);

private:
    QComboBox* _cb;
};

struct PropertyFieldColor : public PropertyFieldText {
    PropertyFieldColor()
    {
        _pb = new QPushButton(this);
        _pb->setText("...");
        _pb->setFixedSize(24, 24);
        _pb->setFlat(true);
        _pb->setProperty("type", "PropertyFieldPath");
        layout()->addWidget(_pb);

        _cd = new QColorDialog(this);

        connect(_pb, &QPushButton::pressed, [&] {
            _cd->setCurrentColor(QColor(_le->text()));
            if (_cd->exec() == QDialog::Accepted) {
                _le->setText(_cd->selectedColor().name(QColor::HexRgb).toUpper());
            }
        });
    }

private:
    QPushButton* _pb;
    QColorDialog* _cd;
};
#endif /* !__PROPERTYFIELDS_H */
