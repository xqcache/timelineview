#pragma once

#include "item/timelineitem.h"
#include "timelinemediautil.h"

namespace tl {

class TimelineAudioItem : public TimelineItem {
public:
    enum PropertyRole {
        MediaInfoRole = userRole(0),
        PathRole = userRole(1),
    };

    enum Type {
        Type = UserType + 7
    };
    using MediaInfo = TimelineMediaUtil::MediaInfo;

    TimelineAudioItem(ItemID item_id, TimelineModel* model);

    void setMediaInfo(const MediaInfo& media_info);
    inline const MediaInfo& mediaInfo() const;

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
    MediaInfo media_info_;
};

inline const TimelineAudioItem::MediaInfo& TimelineAudioItem::mediaInfo() const
{
    return media_info_;
}

inline QString TimelineAudioItem::path() const
{
    return media_info_.path;
}

} // namespace tl