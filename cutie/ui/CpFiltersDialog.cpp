#include "CpFiltersDialog.h"
#include "ui_CpFiltersDialog.h"

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>

CpFiltersDialog::CpFiltersDialog(QWidget* parent, QProxyItem* proxy)
	: m_Ui{ new Ui::FiltersDialog() }, m_Proxy{ proxy } {
	m_Ui->setupUi(this);

	init();
	connectSignals();
}

CpFiltersDialog::~CpFiltersDialog() {
	delete m_Ui;
	m_Ui = nullptr;
}

std::shared_ptr<iphelper::filter_interface> CpFiltersDialog::filter() const {
	TFilterType type = currentFilterType();

	switch (type) {
	case TFilterType::None: {
		return std::make_shared<NoneFilter>();
	}
	case TFilterType::All: {
		return std::make_shared<AllFilter>();
	}
	case TFilterType::Name: {
		std::wstring str = m_Ui->patternLineEdit->text().toStdWString();
		TNameFilterType nameFilterType =
			static_cast<TNameFilterType>(
				m_Ui->nameFilterTypeCombo->itemData(m_Ui->nameFilterTypeCombo->currentIndex(), Qt::UserRole).toInt()
				);
		TStringMatchType stringMatchType =
			static_cast<TStringMatchType>(
				m_Ui->stringMatchTypeCombo->itemData(m_Ui->stringMatchTypeCombo->currentIndex(), Qt::UserRole).toInt()
				);
		bool caseSensitive = m_Ui->caseSensitiveCheckBox->isChecked();

		return std::make_shared<NameFilter>(nameFilterType, str, stringMatchType, caseSensitive);
	}
	case TFilterType::ProcessId: {
		static_assert(sizeof(ProcessIdFilter::TProcessId) == 4, "rework this fn");
		auto convertStringToIdSet = [](const QString& input) -> std::set<ProcessIdFilter::TProcessId> {
			if (input.isEmpty()) {
				return { };
			}

			std::set<ProcessIdFilter::TProcessId> result{ };
			QStringList parts = input.split(';', Qt::SkipEmptyParts);

			for (auto& part : parts) {
				bool isOk{ };
				int v = part.toUInt(&isOk);
				if (!isOk) {
					continue;
				}

				result.insert(v);
			}

			return result;
			};

		return std::make_shared<ProcessIdFilter>(convertStringToIdSet(m_Ui->idsLineEdit->text()));
	}
	}

	return { };
}

void CpFiltersDialog::init() {
	m_Ui->filterTypeCombo->addItem(QObject::tr("None"), QVariant(static_cast<int>(TFilterType::None)));
	m_Ui->filterTypeCombo->addItem(QObject::tr("All"), QVariant(static_cast<int>(TFilterType::All)));
	m_Ui->filterTypeCombo->addItem(QObject::tr("Name"), QVariant(static_cast<int>(TFilterType::Name)));
	m_Ui->filterTypeCombo->addItem(QObject::tr("Process Id"), QVariant(static_cast<int>(TFilterType::ProcessId)));

	m_Ui->nameFilterTypeCombo->addItem(QObject::tr("Process Name"), QVariant(static_cast<int>(TNameFilterType::ProcessName)));
	m_Ui->nameFilterTypeCombo->addItem(QObject::tr("Image Path"), QVariant(static_cast<int>(TNameFilterType::ImagePath)));

	m_Ui->stringMatchTypeCombo->addItem(QObject::tr("Exact"), QVariant(static_cast<int>(TStringMatchType::Exact)));
	m_Ui->stringMatchTypeCombo->addItem(QObject::tr("Contains"), QVariant(static_cast<int>(TStringMatchType::Contains)));
	m_Ui->stringMatchTypeCombo->addItem(QObject::tr("Regular Expression"), QVariant(static_cast<int>(TStringMatchType::RegExp)));

	m_FilterTypeToLayout[TFilterType::Name] = m_Ui->nameFilterGroupBox;
	m_FilterTypeToLayout[TFilterType::ProcessId] = m_Ui->processIdGroupBox;

	for (auto& filter : m_Proxy->filters()) {
		m_Ui->filterTypeCombo->setCurrentIndex(
			static_cast<int>(filterTypeByObject(filter))
		);
		filterTypeChanged();
	}
}

CpFiltersDialog::TFilterType CpFiltersDialog::filterTypeByObject(const std::shared_ptr<iphelper::filter_interface>& filter) {
	auto ptr = filter.get();
	if (dynamic_cast<AllFilter*>(ptr)) {
		return TFilterType::All;
	}

	if (dynamic_cast<NoneFilter*>(ptr)) {
		return TFilterType::None;
	}

	if (dynamic_cast<NameFilter*>(ptr)) {
		return TFilterType::Name;
	}

	if (dynamic_cast<ProcessIdFilter*>(ptr)) {
		return TFilterType::ProcessId;
	}

	return TFilterType::None; // an actual filter type
}

CpFiltersDialog::TFilterType CpFiltersDialog::currentFilterType() const {
	bool isOk{ };
	int itype = m_Ui->filterTypeCombo->itemData(m_Ui->filterTypeCombo->currentIndex(), Qt::UserRole).toInt(&isOk);
	if (!isOk) {
		throw std::runtime_error("smth is wrong");
	}

	TFilterType type = static_cast<TFilterType>(itype);
	switch (type) {
	case TFilterType::None:
	case TFilterType::All:
	case TFilterType::Name:
	case TFilterType::ProcessId:
		break;
	default: throw std::runtime_error("no such filter type");
	}

	return type;
}

void CpFiltersDialog::filterTypeChanged() {
	TFilterType type = currentFilterType();

	for (auto& [filterType, groupBox] : m_FilterTypeToLayout) {
		if (!groupBox) {
			continue;
		}

		groupBox->setHidden(true);
	}
 
	if (QGroupBox* groupBox = m_FilterTypeToLayout[type]
		; groupBox) {
		groupBox->setVisible(true);
	}

	adjustSize();
}

void CpFiltersDialog::connectSignals() {
	QObject::connect(m_Ui->closeButton, &QPushButton::clicked, this, &QDialog::reject);
	QObject::connect(m_Ui->applyChangesButton, &QPushButton::clicked, this, &QDialog::accept);

	QObject::connect(m_Ui->filterTypeCombo, &QComboBox::currentIndexChanged, [this]() -> void {
		filterTypeChanged();
		});
}