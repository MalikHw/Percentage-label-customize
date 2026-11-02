#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(PctPlayLayer, PlayLayer) {
	struct Fields {
		CCLabelTTF* ttfLabel = nullptr;
		std::string ttfPath;
	};

	void setupHasCompleted() {
		PlayLayer::setupHasCompleted();
		this->restylePercentage();
	}

	void updateProgressbar() {
		PlayLayer::updateProgressbar();
		this->restylePercentage();
	}

	void restylePercentage() {
		auto label = m_percentageLabel;
		if (!label) return;
		auto mod = Mod::get();

		float size = mod->getSettingValue<double>("size");
		auto color = mod->getSettingValue<ccColor3B>("color");
		auto font = mod->getSettingValue<std::string>("font");
		auto pos = mod->getSettingValue<std::string>("position");
		auto opacity = static_cast<GLubyte>(mod->getSettingValue<int64_t>("opacity") * 255 / 100);

		// custom ttf, needs a separate label
		std::string ttfPath;
		if (font == "Custom") {
			auto path = mod->getSettingValue<std::filesystem::path>("custom-font");
			std::error_code ec;
			if (!path.empty() && std::filesystem::exists(path, ec)) ttfPath = path.string();
			else font = "bigFont";
		}

		CCNode* target = label;
		CCRGBAProtocol* rgba = label;
		auto f = m_fields.self();

		if (!ttfPath.empty()) {
			if (f->ttfLabel && f->ttfPath != ttfPath) {
				f->ttfLabel->removeFromParent();
				f->ttfLabel = nullptr;
			}
			if (!f->ttfLabel) {
				f->ttfLabel = CCLabelTTF::create(label->getString(), ttfPath.c_str(), 60.f);
				f->ttfLabel->setID("ttf-percentage-label"_spr);
				f->ttfPath = ttfPath;
				label->getParent()->addChild(f->ttfLabel, label->getZOrder());
			}
			if (strcmp(f->ttfLabel->getString(), label->getString()) != 0)
				f->ttfLabel->setString(label->getString());

			f->ttfLabel->setVisible(label->isVisible());
			label->setOpacity(0); // original stays in the tree, just invisible
			target = f->ttfLabel;
			rgba = f->ttfLabel;
		} else {
			if (f->ttfLabel) {
				f->ttfLabel->removeFromParent();
				f->ttfLabel = nullptr;
			}
			auto fnt = font + ".fnt";
			if (strcmp(label->getFntFile(), fnt.c_str()) != 0) label->setFntFile(fnt.c_str());
		}

		target->setScale(size);
		rgba->setColor(color);
		rgba->setOpacity(opacity);

		auto win = CCDirector::get()->getWinSize();
		float margin = 10.f;
		float ax = 0.5f, ay = 0.5f, x = win.width / 2, y = win.height / 2;

		if (pos.starts_with("Top")) { ay = 1.f; y = win.height - margin; }
		else if (pos.starts_with("Bottom")) { ay = 0.f; y = margin; }

		if (pos.ends_with("Left")) { ax = 0.f; x = margin; }
		else if (pos.ends_with("Right")) { ax = 1.f; x = win.width - margin; }

		target->setAnchorPoint({ax, ay});
		target->setPosition({x, y});
	}
};
