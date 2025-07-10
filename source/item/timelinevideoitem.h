#pragma once

#include "item/timelineitem.h"
#include "timelinemediautil.h"

namespace tl {

class TimelineVideoItem : public TimelineItem {
public:
    enum PropertyRole {
        MediaInfoRole = userRole(0),
        PathRole = userRole(1),
    };

    enum Type {
        Type = UserType + 4
    };
    using MediaInfo = TimelineMediaUtil::MediaInfo;

    TimelineVideoItem(ItemID item_id, TimelineModel* model);

    void setMediaInfo(const MediaInfo& media_info);
    inline MediaInfo mediaInfo() const;

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

inline TimelineVideoItem::MediaInfo TimelineVideoItem::mediaInfo() const
{
    return media_info_;
}

inline QString TimelineVideoItem::path() const
{
    return media_info_.path;
}

} // namespace tl