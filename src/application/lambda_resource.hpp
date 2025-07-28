#pragma once

#include <Wt/WResource.h>

namespace LambdaSnail::application
{

/**
 * A WT resource that serves requests from a lambda function. The lambda is simply copied in the
 * ctor, so any resources used by the lambda must have a longer lifetime than the resource object
 * itself, otherwise the behavior is undefined.
 * @tparam TLambda The lambda used to serve requests.
 * @see https://webtoolkit.eu/widgets/media/resources
 */
class LambdaResource : public Wt::WResource
{
  public:
    using TFunc = std::function<void(Wt::Http::Request const& request, Wt::Http::Response& response)>;

    explicit LambdaResource(TFunc&& lambda, std::string const&& fileNameHint);

    void handleRequest(Wt::Http::Request const& request, Wt::Http::Response& response) override;

  private:
    TFunc m_Lambda;
};

}