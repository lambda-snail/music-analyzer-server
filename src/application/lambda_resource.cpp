#include "lambda_resource.hpp"

namespace LambdaSnail::application
{
LambdaResource::LambdaResource(TFunc&& lambda, std::string const&& fileNameHint)
    : m_Lambda(std::move(lambda))
{
    suggestFileName(fileNameHint);
}

void LambdaResource::handleRequest(Wt::Http::Request const& request, Wt::Http::Response& response)
{
    m_Lambda(request, response);
}
} // namespace LambdaSnail::application