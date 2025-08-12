#pragma once

#include "item/timelineitem.h"
#include "timelinemediautil.h"

namespace tl {

class TimelineAudioItem : public TimelineItem {
public:
    enum PropertyRole {
        AudioInfoRole = userRole(0),
        PathRole = userRole(1),
    };

    enum Type {
        Type = UserType + 7
    };
    using AudioInfo = TimelineMediaUtil::AudioInfo;

    TimelineAudioItem(ItemID item_id, TimelineModel* model);

    void setAudioInfo(const AudioInfo& audio_info);
    inline const AudioInfo& audioInfo() const;

    void setPath(const QString& path);
    inline QString path() const;

public:
    int type() const override;
    const char* typeName() const override;
    QString toolTip() const override;
    bool load(const nlohmann::json& j) override;
    nlohmann::json save() const override;

    bool setProperty(int role, const QVariant& value) override;
    std::optional<QVariant> property(int role) const override;

private:
    AudioInfo audio_info_;
};

inline const TimelineAudioItem::AudioInfo& TimelineAudioItem::audioInfo() const
{
    return audio_info_;
}

inline QString TimelineAudioItem::path() const
{
    return audio_info_.path;
}

} // namespace tl